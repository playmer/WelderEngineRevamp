// MIT Licensed (see LICENSE.md).
const execa = require('execa');
const path = require('path');
const mkdirp = require('mkdirp');
const fs = require('fs');
const glob = require('glob');
const os = require('os');
const request = require('request');
const rimraf = require('rimraf');
const mv = require('mv');
const ncp = require('ncp');
const performance = require('perf_hooks').performance;
const extract = require('extract-zip');
const archiver = require('archiver');
const commandExists = require('command-exists').sync;

const bytesPerMb = 1024 * 1024;

let hostos;
let executableExtension = '';

function initialize()
{
  switch (os.platform())
  {
  case 'win32':
    hostos = 'windows';
    executableExtension = '.exe';
    break;
  case 'darwin':
    hostos = 'mac';
    break;
  default:
    hostos = 'linux';
    break;
  }
}

initialize();

const dirs = (() =>
{
  const root = process.cwd();
  const libraries = path.join(root, 'Libraries');
  const temp = path.join(root, 'Temp');
  const tempBuild = path.join(temp, 'Build');
  const tempDownload = path.join(temp, 'Download');
  const tempDoxygen = path.join(temp, 'Doxygen');
  const tempTest = path.join(temp, 'Test');
  const tempTools = path.join(temp, 'Tools');
  const tempImages = path.join(temp, 'Images');
  const localNinja = path.join(tempTools, 'Ninja');
  const localLlvm = path.join(tempTools, 'Llvm');
  const localDoxygen = path.join(tempTools, 'Doxygen');
  const localCmake = path.join(tempTools, 'Cmake');
  const localNinjaBin = path.join(localNinja, 'bin');
  const localLlvmBin = path.join(localLlvm, 'bin');
  const localDoxygenBin = path.join(localDoxygen, 'bin');
  const localCmakeBin = path.join(localCmake, 'bin');

  return {
    root,
    temp,
    libraries,
    tempBuild,
    tempDownload,
    tempDoxygen,
    tempTest,
    tempTools,
    tempImages,
    localNinja,
    localLlvm,
    localDoxygen,
    localCmake,
    localNinjaBin,
    localLlvmBin,
    localDoxygenBin,
    localCmakeBin,
  };
})();

const paths = (() =>
{
  const ninja = path.join(dirs.localNinjaBin, 'ninja' + executableExtension);
  const clang = path.join(dirs.localLlvmBin, 'clang' + executableExtension);
  const clangXX = path.join(dirs.localLlvmBin, 'clang++' + executableExtension);
  const lld = path.join(dirs.localLlvmBin, 'lld' + executableExtension);
  const llvmAr = path.join(dirs.localLlvmBin, 'llvm-ar' + executableExtension);
  const clangTidy = path.join(dirs.localLlvmBin, 'clang-tidy' + executableExtension);
  const clangFormat = path.join(dirs.localLlvmBin, 'clang-format' + executableExtension);
  const doxygen = path.join(dirs.localDoxygenBin, 'doxygen' + executableExtension);
  const cmake = path.join(dirs.localCmakeBin, 'cmake' + executableExtension);

  // Commands that we don't have portable/image versions of:
  const git = 'git';

  return {
    ninja,
    clang,
    clangXX,
    lld,
    llvmAr,
    clangTidy,
    clangFormat,
    doxygen,
    cmake,
    git,
  };
})();

function makeDir(dirPath)
{
  mkdirp.sync(dirPath, {
    recursive: true
  });
  return dirPath;
}

function makeAllDirs()
{
  // Ensuring all directories exist just makes everything easier.
  for (const key in dirs)
  {
    makeDir(dirs[key]);
  }
}

//function addPath(directory)
//{
//  process.env.PATH += `;${directory}`;
//}

function sleep(ms)
{
  return new Promise(resolve =>
  {
    setTimeout(resolve, ms);
  });
}

function printIndented(text, printer, symbol)
{
  if (!text)
  {
    return;
  }

  if (text.stack)
  {
    text = `${text.stack}`;
  }
  else
  {
    text = `${text}`;
  }

  const indent = ' '.repeat(8) + symbol;
  const matches = text.match(/[^\r\n]+/g);
  if (matches)
  {
    for (const line of matches)
    {
      printer(indent + line);
    }
  }
  else if (text)
  {
    printer(indent + text);
  }
}

function printError(text)
{
  printIndented(text, console.error, '- ');
}

function printLog(text)
{
  printIndented(text, console.log, '+ ');
}

async function exec(executable, args, options)
{
  // A file may be locked after it completes downloading.
  // This may be a result of a virus scanner or other process.
  let lastErr = 'Unknown';
  let result = null;
  for (let attempts = 10; attempts >= 0; --attempts)
  {
    try
    {
      result = await execa(executable, args, options);
      break;
    }
    catch (err)
    {
      lastErr = err;
      await sleep(100);
      continue;
    }
  }

  if (result)
  {
    if (options.err)
    {
      options.err(result.stderr);
    }
    if (options.out)
    {
      options.out(result.stdout);
    }
  }
  else
  {
    printError(lastErr);
  }
  return result;
}

async function execStdout(...args)
{
  return (await exec(...args)).stdout;
}

function verifyFileExists(filePath)
{
  const exists = path.isAbsolute(filePath) ? fs.existsSync(filePath) : commandExists(filePath);

  if (!exists)
  {
    printError(`file/command '${filePath}' does not exist`);
    return false;
  }
  return true;
}

function getPercent(count, total)
{
  return total !== 0 ? Math.round((count / total) * 100) : '?';
}

function download(url, filePath)
{
  console.log(`Downloading '${url}'`);
  return new Promise(resolve =>
  {
    const file = fs.createWriteStream(filePath);

    let expectedBytes = 0;
    let downloadedBytes = 0;
    let lastBytes = 0;
    let lastTime = performance.now();
    request(url)
      .on('error', printError)
      .on('response', data =>
      {
        expectedBytes = data.headers['content-length'];
      })
      .on('data', chunk =>
      {
        downloadedBytes += chunk.length;
        const now = performance.now();
        if (now - lastTime >= 1000)
        {
          const deltaSeconds = (now - lastTime) / 1000.0;
          const deltaMb = (downloadedBytes - lastBytes) / bytesPerMb;

          lastTime = now;
          lastBytes = downloadedBytes;

          const mb = Math.round(downloadedBytes / bytesPerMb);
          const mbps = deltaMb / deltaSeconds;
          const percent = getPercent(downloadedBytes, expectedBytes);

          console.log(` - ${percent}% - ${mb}mb (${mbps} MB/s)`);
        }
      })
      .pipe(file);

    file.on('finish', () =>
    {
      file.close();
      resolve();
      console.log(`Completed '${url}'`);
    });
  });
}

async function downloadTemp(url, fileName)
{
  const filePath = path.join(dirs.tempDownload, fileName);
  try
  {
    fs.unlinkSync(filePath);
  }
  catch (err)
  {
    // Ignored.
  }

  await download(url, filePath);

  return filePath;
}

async function standardInstall(filePath, args)
{
  const options = {
    stdio: ['ignore', 'pipe', 'pipe'],
    out: printLog,
    err: printError,
    reject: false
  };
  await exec(filePath, args ? args : [], options);
}

function getSingleSubDirectory(parentDir)
{
  let singleDirectory = null;
  let directories = 0;
  let files = 0;
  for (const name of fs.readdirSync(parentDir))
  {
    const checkPath = path.join(parentDir, name);
    if (fs.lstatSync(checkPath).isDirectory())
    {
      singleDirectory = checkPath;
      ++directories;
    }
    else
    {
      ++files;
    }
  }

  if (directories === 1 && files === 0)
  {
    return singleDirectory;
  }
  return null;
}

async function tarExtractor(filePath, tempExtractDir)
{
  const options = {
    stdio: ['ignore', 'pipe', 'pipe'],
    out: printLog,
    err: printError,
    reject: false
  };
  const args = ['xf', filePath, '-C', tempExtractDir];
  return await exec('tar', args, options);
}

async function zipExtractor(filePath, tempExtractDir)
{
  const options = {
    dir: tempExtractDir
  };
  return await new Promise(resolve =>
  {
    extract(filePath, options, err =>
    {
      printError(err);
      resolve();
    });
  });
}

async function extractInstall(extractor, filePath, extractDir)
{
  const tempExtractDir = path.join(dirs.tempDownload, 'extract');
  mkdirp.sync(tempExtractDir);

  await extractor(filePath, tempExtractDir);

  // Check if there is a single directory. If so we're going to move it.
  const singleSubDir = getSingleSubDirectory(tempExtractDir);
  const moveDir = singleSubDir ? singleSubDir : tempExtractDir;

  rimraf.sync(extractDir);

  const mvOptions = {
    mkdirp: true
  };
  const error = await new Promise(resolve => mv(moveDir, extractDir, mvOptions, resolve));
  printError(error);

  rimraf.sync(tempExtractDir);
}

async function tarInstall(filePath, extractDir)
{
  return await extractInstall(tarExtractor, filePath, extractDir);
}

async function zipInstall(filePath, extractDir)
{
  return await extractInstall(zipExtractor, filePath, extractDir);
}

async function installProgram(info)
{
  if (info.check)
  {
    console.log(`Checking ${info.name}`);
    if (await info.check())
    {
      console.log(`Checked ${info.name}`);
      return;
    }
  }

  const settings = info[hostos];
  if (settings)
  {
    let filePath = null;
    if (settings.url && settings.file)
    {
      filePath = await downloadTemp(settings.url, settings.file);
    }

    console.log(`Installing ${info.name}`);
    await settings.install(filePath);
    console.log(`Completed ${info.name}`);
  }
  else
  {
    printError(`installProgram: Unhandled hostos ${hostos} for ${info.name}`);
  }

  if (info.check)
  {
    if (await info.check())
    {
      console.log(`Checked ${info.name}`);
    }
    else
    {
      printError(`Failed ${info.name}`);
    }
  }
}

function installNinja()
{
  return installProgram({
    name: 'Ninja',
    check: async () => fs.existsSync(paths.ninja),
    windows: {
      url: 'https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-win.zip',
      file: 'ninja-win.zip',
      install: filePath => zipInstall(filePath, dirs.localNinjaBin)
    },
    linux: {
      url: 'https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-linux.zip',
      file: 'ninja-linux.zip',
      install: filePath => zipInstall(filePath, dirs.localNinjaBin)
    },
  });
}

function installLlvm()
{
  return installProgram({
    name: 'Llvm',
    check: async () => fs.existsSync(paths.clang) && fs.existsSync(paths.clangTidy) && fs.existsSync(paths.clangFormat),
    windows: {
      url: 'http://releases.llvm.org/7.0.0/LLVM-7.0.0-win64.exe',
      file: 'LLVM-7.0.0-win64.exe',
      install: async filePath =>
      {
        // Do a standard install, then copy the artifacts to our local directory.
        const installPath = 'C:\\Program Files\\LLVM';
        await standardInstall(filePath, ['/S', '/D=' + installPath]);
        await new Promise(resolve =>
        {
          ncp(installPath, dirs.localLlvm, err =>
          {
            printError(err);
            resolve();
          });
        });
      }
    },
    linux: {
      url: 'http://releases.llvm.org/7.0.0/clang+llvm-7.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz',
      file: 'clang+llvm-7.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz',
      install: filePath => tarInstall(filePath, dirs.localLlvm)
    },
  });
}

function installDoxygen()
{
  return installProgram({
    name: 'Doxygen',
    check: async () => fs.existsSync(paths.doxygen),
    windows: {
      url: 'http://doxygen.nl/files/doxygen-1.8.15.windows.x64.bin.zip',
      file: 'doxygen-1.8.15.windows.x64.bin.zip',
      install: filePath => zipInstall(filePath, dirs.localDoxygenBin)
    },
    linux: {
      url: 'http://doxygen.nl/files/doxygen-1.8.15.linux.bin.tar.gz',
      file: 'doxygen-1.8.15.linux.bin.tar.gz',
      install: filePath => tarInstall(filePath, dirs.localDoxygen)
    },
  });
}

function installCmake()
{
  return installProgram({
    name: 'CMake',
    check: async () => fs.existsSync(paths.cmake),
    windows: {
      url: 'https://github.com/Kitware/CMake/releases/download/v3.13.2/cmake-3.13.2-win64-x64.zip',
      file: 'cmake-3.13.2-win64-x64.zip',
      install: filePath => zipInstall(filePath, dirs.localCmake)
    },
    linux: {
      url: 'https://github.com/Kitware/CMake/releases/download/v3.13.2/cmake-3.13.2-Linux-x86_64.tar.gz',
      file: 'cmake-3.13.2-Linux-x86_64.tar.gz',
      install: filePath => tarInstall(filePath, dirs.localCmake)
    },
  });
}

function safeDeleteFile(filePath)
{
  try
  {
    fs.unlinkSync(filePath);
  }
  catch (err)
  {
    // Ignored.
  }
}

function gatherSourceFiles()
{
  console.log('Gathering Source Files');
  const files = glob.sync('**/*.@(c|cc|cxx|cpp|h|hxx|hpp|inl)', {
    cwd: dirs.libraries
  });

  for (let i = 0; i < files.length;)
  {
    const fullPath = path.join(dirs.libraries, files[i]);
    const fileOptions =
    {
      encoding: 'utf8'
    };
    const code = fs.readFileSync(fullPath, fileOptions);

    if (code.startsWith('// External.'))
    {
      files.splice(i, 1);
    }
    else
    {
      ++i;
    }
  }
  return files;
}

async function runEslint(options)
{
  console.log('Running Eslint');
  const eslintOptions = {
    cwd: dirs.root,
    stdio: ['ignore', 'pipe', 'pipe'],
    out: options.validate ? printError : printLog,
    err: options.validate ? printError : printLog,
    reject: false
  };
  const args = ['node_modules/eslint/bin/eslint.js', '.'];

  if (!options.validate)
  {
    args.push('--fix');
  }

  await exec('node', ['node_modules/eslint/bin/eslint.js', '.'], eslintOptions);
}

async function runClangTidy(options, sourceFiles)
{
  console.log('Running Clang Tidy');
  if (!verifyFileExists(paths.clangTidy))
  {
    return;
  }

  // Run clang-tidy.
  const clangTidyOptions = {
    cwd: dirs.libraries,
    // We only ignore stderr because it prints 'unable to find compile_commands.json'.
    stdio: ['ignore', 'pipe', 'inherit'],
    reject: false
  };
  for (const filePath of sourceFiles)
  {
    const fileOptions =
    {
      encoding: 'utf8'
    };
    const fullPath = path.join(dirs.libraries, filePath);
    const oldCode = fs.readFileSync(fullPath, fileOptions);

    // We always tell it to fix the file, and we compare it afterward to see if it changed.
    const args = ['-extra-arg=-Weverything', '-fix'/*, '-p='*/, '-header-filter=.*', filePath];

    // Clang-tidy emits all the errors to the standard out.
    // We capture them and re-emit them to stderr.
    const result = await exec(paths.clangTidy, args, clangTidyOptions);

    if (!options.validate)
    {
      continue;
    }

    const newCode = fs.readFileSync(fullPath, fileOptions);
    if (oldCode !== newCode)
    {
      printError(`File '${fullPath}' was not clang-tidy'd`);
      printError(result.stdout);

      // Rewrite the original code back.
      fs.writeFileSync(fullPath, oldCode, fileOptions);
    }
  }
}

async function runClangFormat(options, sourceFiles)
{
  console.log('Running Clang Format');
  if (!verifyFileExists(paths.clangFormat))
  {
    return;
  }

  const clangFormatOptions = {
    cwd: dirs.libraries,
    stdio: ['ignore', 'pipe', 'ignore'],
    reject: false,
    stripFinalNewline: false,
    stripEof: false
  };

  for (const filePath of sourceFiles)
  {
    // Clang-format emits the formatted file to the output.
    // In this build script we want to emit errors if the user
    // did not auto-format their code, so we will perform a diff.
    const result = await exec(paths.clangFormat, [filePath], clangFormatOptions);

    const fullPath = path.join(dirs.libraries, filePath);
    const fileOptions =
    {
      encoding: 'utf8'
    };
    const oldCode = fs.readFileSync(fullPath, fileOptions);
    const newCode = result.stdout;

    if (options.validate)
    {
      printError(`File '${fullPath}' was not clang-formatted`);
    }
    else if (oldCode !== newCode)
    {
      fs.writeFileSync(fullPath, newCode, fileOptions);
    }
  }
}

async function runWelderFormat(options, sourceFiles)
{
  console.log('Running Welder Format');

  for (const filePath of sourceFiles)
  {
    const fullPath = path.join(dirs.libraries, filePath);
    const fileOptions =
    {
      encoding: 'utf8'
    };
    const oldCode = fs.readFileSync(fullPath, fileOptions);

    // Split our code into lines (detect Windows newline too so we can remove it).
    const lines = oldCode.split(/\r?\n/);

    const commentRegex = /^[/*-=\\]+.*/;

    // Remove any comments from the first lines.
    while (lines.length !== 0)
    {
      const line = lines[0];
      if (commentRegex.test(line))
      {
        lines.shift();
      }
      else
      {
        break;
      }
    }

    // Remove any comments that are long bar comments.
    // Technically this could remove the beginning of a multi-line comment, but our
    // style says it's invalid to have one that has a ton of stars in it anyways.
    const barCommentRegex = /^[/*-=\\]{40}.*/;

    // These comments may have text after them, but we delete that too intentionally.
    for (let i = 0; i < lines.length;)
    {
      const line = lines[i];
      if (barCommentRegex.test(line))
      {
        lines.splice(i, 1);
      }
      else
      {
        ++i;
      }
    }

    // Add back in the standard file header (would have been removed above).
    lines.unshift('// MIT Licensed (see LICENSE.md).');

    // Join all lines together with a standard UNIX newline.
    const newCode = lines.join('\n');

    if (options.validate)
    {
      printError(`File '${fullPath}' must be welder-formatted`);
    }
    else if (oldCode !== newCode)
    {
      fs.writeFileSync(fullPath, newCode, fileOptions);
    }
  }
}

/*
async function runDoxygen()
{
  console.log('Running Doxygen');
  if (!verifyFileExists(paths.doxygen))
  {
    return;
  }

  const doxygenOptions = {
    cwd: dirs.root,
    stdio: ['ignore', 'pipe', 'pipe'],
    out: printLog,
    err: printError,
    reject: false
  };
  await exec(paths.doxygen, [], doxygenOptions);
}
*/

async function runCmake(options)
{
  console.log('Running Cmake');
  makeAllDirs();

  if (!verifyFileExists(paths.cmake) || !verifyFileExists(paths.git))
  {
    return null;
  }

  const gitOptions = {
    cwd: dirs.root,
    stdio: ['ignore', 'pipe', 'pipe'],
    err: printError,
    reject: false
  };
  const revision = await execStdout(paths.git, ['rev-list', '--count', 'HEAD'], gitOptions);
  const shortChangeset = await execStdout(paths.git, ['log', '-1', '--pretty=%h', '--abbrev=12'], gitOptions);
  const changeset = await execStdout(paths.git, ['log', '-1', '--pretty=%H'], gitOptions);
  const changesetDate = `"${await execStdout(paths.git, ['log', '-1', '--pretty=%cd', '--date=format:%Y-%m-%d'], gitOptions)}"`;

  const aliases =
  {
    windows:
    {
      builder: 'Visual Studio 15 2017',
      toolchain: 'MSVC',
      platform: 'Windows',
      architecture: 'X64',
      configuration: 'Any',
      targetos: 'Windows',
    },
    Linux:
    {
      builder: 'Ninja',
      toolchain: 'Clang',
      platform: 'SDLSTDEmpty',
      architecture: 'ANY',
      config: 'Release',
      targetos: 'Linux',
    },
    emscripten:
    {
      builder: 'MinGW Makefiles',
      toolchain: 'Emscripten',
      platform: 'Emscripten',
      architecture: 'WASM',
      configuration: 'Release',
      targetos: 'Emscripten',
    },
    empty:
    {
      builder: 'Ninja',
      toolchain: 'Clang',
      platform: 'Stub',
      architecture: 'ANY',
      configuration: 'Release',
      targetos: hostos,
    },
  };

  const combo = aliases[options.alias ? options.alias : hostos];

  // Allow options to override builder, toolchian, etc.
  // It is the user's responsibility to ensure this is a valid combination.
  Object.assign(combo, options);

  let builderArgs = [];
  let toolchainArgs = [];
  let architectureArgs = [];
  const configArgs = [];

  if (combo.builder === 'Ninja')
  {
    builderArgs.push('-DCMAKE_MAKE_PROGRAM=ninja');
  }

  if (combo.toolchain === 'Emscripten')
  {
    if (!process.env.EMSCRIPTEN)
    {
      printError('Cannot find EMSCRIPTEN environment variable');
    }

    const toolchainFile = path.join(process.env.EMSCRIPTEN, 'cmake/Modules/Platform/Emscripten.cmake');
    toolchainArgs.push(`-DCMAKE_TOOLCHAIN_FILE=${toolchainFile}`);
    toolchainArgs.push('-DEMSCRIPTEN_GENERATE_BITCODE_STATIC_LIBRARIES=1');
  }

  if (combo.toolchain === 'Clang')
  {
    if (hostos === 'Windows')
    {
      // CMake on Windows tries to do a bunch of detection thinking that it will be using MSVC.
      toolchainArgs.push('-DCMAKE_SYSTEM_NAME=Generic');
    }

    toolchainArgs.push('-DCMAKE_C_COMPILER:PATH=clang');
    toolchainArgs.push('-DCMAKE_CXX_COMPILER:PATH=clang++');
    toolchainArgs.push('-DCMAKE_C_COMPILER_ID=Clang');
    toolchainArgs.push('-DCMAKE_CXX_COMPILER_ID=Clang');
    toolchainArgs.push('-DCMAKE_LINKER=lld');
    toolchainArgs.push('-DCMAKE_AR=/usr/bin/llvm-ar');
  }

  if (combo.toolchain === 'MSVC' && combo.architecture === 'X64')
  {
    architectureArgs.push('-DCMAKE_GENERATOR_PLATFORM=x64');
    architectureArgs.push('-T');
    architectureArgs.push('host=x64');
  }

  if (combo.toolchain !== 'MSVC')
  {
    configArgs.push(`-DCMAKE_BUILD_TYPE=${combo.config}`);
    configArgs.push('-DCMAKE_EXPORT_COMPILE_COMMANDS=1');
  }

  const cmakeArgs = [
    `-DWELDER_REVISION=${revision}`,
    `-DWELDER_SHORT_CHANGESET=${shortChangeset}`,
    `-DWELDER_CHANGESET=${changeset}`,
    `-DWELDER_CHANGESET_DATE=${changesetDate}`,
    '-G', combo.builder,
    ...builderArgs,
    `-DWELDER_TOOLCHAIN=${combo.toolchain}`,
    ...toolchainArgs,
    `-DWELDER_PLATFORM=${combo.platform}`,
    `-DWELDER_ARCHITECTURE=${combo.architecture}`,
    ...architectureArgs,
    ...configArgs,
    `-DWELDER_HOSTOS=${hostos}`,
    `-DWELDER_TARGETOS=${combo.targetos}`,
    dirs.root,
  ];

  console.log(cmakeArgs);
  console.log(combo);

  const comboString = `${combo.builder}_${combo.toolchain}_${combo.platform}_${combo.architecture}_${combo.configuration}`.replace(/ /g, '-');
  const comboDir = path.join(dirs.tempBuild, comboString);
  rimraf.sync(comboDir);
  makeDir(comboDir);

  const cmakeOptions = {
    cwd: comboDir,
    stdio: ['ignore', 'pipe', 'pipe'],
    out: printLog,
    err: printError,
    reject: false
  };
  await exec(paths.cmake, cmakeArgs, cmakeOptions);

  return comboDir;
}

function safeChmod(file, mode)
{
  try
  {
    fs.chmodSync(file, mode);
  }
  catch (err)
  {
    printError(err);
  }
}

async function runBuild(buildDir, config, testExecutablePaths)
{
  console.log('Running Build');
  if (!verifyFileExists(paths.cmake))
  {
    return;
  }

  const options = {
    cwd: buildDir,
    stdio: ['ignore', 'pipe', 'pipe'],
    out: text =>
    {
      if (text.search(/(FAILED|failed|ERROR)/) === -1)
      {
        printLog(text);
      }
      else
      {
        printError(text);
      }
    },
    err: printError,
    reject: false
  };

  await exec(paths.cmake, ['--build', '.', '--config', config], options);

  const addExecutable = file =>
  {
    if (fs.existsSync(file))
    {
      safeChmod(file, 0o777);
      testExecutablePaths.push(file);
    }
  };

  addExecutable(path.join(buildDir, config, 'ne' + executableExtension));
  addExecutable(path.join(buildDir, 'ne' + executableExtension));
}

/*
async function runTests(testExecutablePaths)
{
  console.log('Running Tests');
  for (const testExecutablePath of testExecutablePaths)
  {
    const options = {
      cwd: path.dirname(testExecutablePath),
      stdio: ['pipe', 'pipe', 'pipe'],
      err: printError,
      reject: false
    };
    await exec(testExecutablePath, [], options);
  }
}
*/

async function createImage()
{
  console.log('Creating');
  makeAllDirs();
  const imagePath = path.join(dirs.tempImages, `image-${hostos}.zip`);
  safeDeleteFile(imagePath);

  const output = fs.createWriteStream(imagePath);
  const archive = archiver('zip', {
    zlib:
    {
      level: 9
    }
  });
  archive.on('error', printError);
  archive.on('warning', printError);

  let lastTime = performance.now();
  archive.on('progress', data =>
  {
    const now = performance.now();
    if (now - lastTime > 1000)
    {
      const percent = getPercent(data.fs.processedBytes, data.fs.totalBytes);
      console.log(` - ${percent}% - ${data.fs.processedBytes} / ${data.fs.totalBytes}`);
      lastTime = now;
    }
  });

  archive.pipe(output);
  archive.directory(dirs.tempTools, false);
  archive.finalize();

  await new Promise(resolve => output.on('close', resolve));
  console.log('Created');
}

async function clean()
{
  console.log('Cleaning');
  rimraf.sync(dirs.temp);
  console.log('Cleaned');
}

async function format(options)
{
  console.log('Formatting');
  makeAllDirs();
  await runEslint(options);
  // TODO(Trevor.Sundberg): Run cmake_format.
  const sourceFiles = gatherSourceFiles();
  if (options.tidy)
  {
    await runClangTidy(options, sourceFiles);
  }
  await runClangFormat(options, sourceFiles);
  await runWelderFormat(options, sourceFiles);
  // TODO(Trevor.Sundberg): Run cppcheck.
  // TODO(Trevor.Sundberg): Run cpplint.'
  //await runDoxygen();
  // TODO(Trevor.Sundberg): Run moxygen.
  console.log('Formatted');
}

async function build(options)
{
  console.log('Building');
  const buildDir = await runCmake(options);
  const testExecutablePaths = [];
  const configuration = options.configuration ? options.configuration : 'Release';
  await runBuild(buildDir, configuration, testExecutablePaths);
  //await runTests(testExecutablePaths);
  console.log('Built');
}

async function installImage()
{
  makeAllDirs();
  await installProgram({
    name: 'Image',
    windows: {
      url: 'https://github.com/WelderFoundation/WelderFiles/releases/download/v0.0.0/image-windows.zip',
      file: 'image-windows.zip',
      install: filePath => zipInstall(filePath, dirs.tempTools)
    },
    linux: {
      url: 'https://github.com/WelderFoundation/WelderFiles/releases/download/v0.0.0/image-linux.zip',
      file: 'image-linux.zip',
      install: filePath => zipInstall(filePath, dirs.tempTools)
    },
  });
}

async function installSources()
{
  makeAllDirs();
  await installNinja();
  await installLlvm();
  await installDoxygen();
  await installCmake();
}

function getParsedArgs()
{
  const args = process.argv.splice(2);

  if (args.length === 0)
  {
    return ['clean', 'installImage', 'build'];
  }

  const json = args.join(' ');

  if (json.startsWith('['))
  {
    return eval(json); // eslint-disable-line
  }

  // Assume the arguments are not in json format (just an array of strings).
  return args;
}

async function main()
{
  const commands = {
    createImage,
    clean,
    installImage,
    installSources,
    format,
    runCmake,
    build,
  };

  const parsedArgs = getParsedArgs();
  for (let arg of parsedArgs)
  {
    console.log('--------------------------------------------------');
    let command;
    if (typeof arg === 'string')
    {
      command = commands[arg];
      arg = {
        name: arg
      };
    }
    else
    {
      command = commands[arg.name];
    }

    if (command)
    {
      await command(arg);
    }
    else
    {
      console.error(`Invalid command '${arg}'`);
    }
  }
}
main();

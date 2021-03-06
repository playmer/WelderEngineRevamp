// MIT Licensed (see LICENSE.md).
#include "Precompiled.hpp"

namespace Zero
{

void LoadContentConfig()
{
  InitializeContentSystem();

  ContentSystem* contentSystem = Z::gContentSystem;

  Cog* configCog = Z::gEngine->GetConfigCog();
  MainConfig* mainConfig = configCog->has(MainConfig);

  Array<String>& librarySearchPaths = contentSystem->LibrarySearchPaths;
  ContentConfig* contentConfig = configCog->has(ContentConfig);
  String appCacheDirectory = GetUserLocalDirectory();

  String sourceDirectory = mainConfig->SourceDirectory;
  ErrorIf(sourceDirectory.Empty(), "Expected a source directory");

  if (contentConfig)
    librarySearchPaths.InsertAt(0, contentConfig->LibraryDirectories.All());

  librarySearchPaths.PushBack(FilePath::Combine(sourceDirectory, "Resources"));

  CreateDirectoryAndParents(FilePath::Combine(appCacheDirectory, GetApplicationName()));

  contentSystem->ToolPath = FilePath::Combine(sourceDirectory, "Tools");

  contentSystem->mHistoryEnabled = contentConfig->HistoryEnabled;

  // To avoid conflicts of assets of different versions(especially when the
  // version selector goes live) set the content folder to a unique directory
  // based upon the version number
  String revisionChangesetName = BuildString("ZeroVersion", GetRevisionNumberString(), "-", GetChangeSetString());
  contentSystem->ContentOutputPath = FilePath::Combine(appCacheDirectory, "ZeroContent", revisionChangesetName);

  // If we don't already have the content output directory, then see if we have
  // local prebuilt content that can be copied into the output directory (this
  // is faster than building the content ourselves, if it exists).
  if (!DirectoryExists(contentSystem->ContentOutputPath))
  {
    String prebuiltContent = FilePath::Combine(sourceDirectory, "PrebuiltZeroContent");
    if (DirectoryExists(prebuiltContent))
    {
      ZPrint("Copying prebuilt content from '%s' to '%s'\n",
             prebuiltContent.c_str(),
             contentSystem->ContentOutputPath.c_str());
      CopyFolderContents(contentSystem->ContentOutputPath, prebuiltContent);
    }
  }
}

bool LoadContentLibrary(StringParam name, bool isCore)
{
  ContentLibrary* library = Z::gContentSystem->Libraries.FindValue(name, nullptr);
  if (!library)
  {
    FatalEngineError("Failed to find core content library %s.\n", name.c_str());
    return false;
  }

  if (isCore)
    library->SetReadOnly(true);

  Status status;
  HandleOf<ResourcePackage> packageHandle = Z::gContentSystem->BuildLibrary(status, library, false);
  ResourcePackage* package = packageHandle;

  if (!status)
    return false;

  if (isCore)
  {
    forRange (ResourceEntry& entry, package->Resources.All())
    {
      if (entry.mLibrarySource)
      {
        if (ContentEditorOptions* options = entry.mLibrarySource->has(ContentEditorOptions))
          entry.mLibrarySource->ShowInEditor = options->mShowInEditor;
        else
          entry.mLibrarySource->ShowInEditor = false;
      }
    }
  }

  Z::gResources->LoadPackage(status, package);
  if (!status)
  {
    if (isCore)
    {
      FatalEngineError("Failed to load core content library for editor. Resources"
                       " need to be in the working directory.");
    }

    DoNotifyError("Failed to load resource package.", status.Message);
  }

  return (bool)status;
}

bool LoadCoreContent(Array<String>& coreLibs)
{
  Z::gContentSystem->EnumerateLibraries();

  ZPrint("Loading Content...\n");

  TimerBlock timer("Loading Content");

  String docDirectory = GetUserDocumentsDirectory();

  LoadContentLibrary("FragmentCore", true);
  LoadContentLibrary("Loading", true);

  forRange (String libraryName, coreLibs.All())
  {
    LoadContentLibrary(libraryName, true);
  }

  // Show all default resources
  forRange (ResourceManager* manager, Z::gResources->Managers.Values())
  {
    if (manager->mCanCreateNew)
      ErrorIf(manager->mExtension.Empty(), "Must set an extension on %s", manager->GetResourceType()->Name.c_str());

    Resource* resource = manager->GetResource(manager->DefaultResourceName, ResourceNotFound::ReturnNull);
    if (resource && resource->mContentItem)
    {
      resource->mContentItem->ShowInEditor = true;

      // Moved default font to the Loading library for progress display
      ErrorIf(resource->mContentItem->mLibrary->Name != "ZeroCore" &&
                  resource->mContentItem->mLibrary->Name != "Loading",
              "Only resources that are in core can be defaults");
    }
    else
    {
      ErrorIf(!manager->mNoFallbackNeeded,
              "Failed to find default resource for resource type %s",
              manager->mResourceTypeName.c_str());
    }
  }

  return true;
}

} // namespace Zero

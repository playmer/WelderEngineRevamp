#include <memory>

#include "source/spirv_target_env.h"
#include "source/spirv_validator_options.h"
#include "spirv-tools/libspirv.hpp"
#include "spirv_glsl.hpp"
#include "spirv_hlsl.hpp"
#include "spirv-tools/optimizer.hpp"

#include "ZilchShadersStandard.hpp"

#include "Events.hpp"

using namespace Zilch;
using namespace Zero;



 struct TypeBuilder
{
  TypeBuilder(LibraryBuilder& aBuilder, BoundType* aType) : mBuilder{&aBuilder}, mType{aType}
  {
  }

  template <auto aFunction>
  void Function(StringParam name //,
                                 // BoundFn function,
                                 // const ParameterArray& parameters,
                                 // Type* returnType,
                                 // FunctionOptions::Flags options,
                                 // NativeVirtualInfo nativeVirtual = NativeVirtualInfo()
  )
  {
    //TemplateBinding::FromMethod<decltype(ZilchNoOverload &Math::Matrix2::GenerateScale),
    //                           Math::Matrix2::GenerateScale>(
    //    builder, core.MathType, "GenerateScaleMatrix2x2", "scale", ZilchNoOverload(&Math::Matrix2::GenerateScale));
  }

  LibraryBuilder* mBuilder;
  BoundType* mType;
};

 void ZilchCompilerErrorCallback(Zilch::ErrorEvent* e)
{
  String& code = e->Location.Code;
  String msg = e->GetFormattedMessage(Zilch::MessageFormat::MsvcCpp);
  ZPrint("%s", msg.c_str());
  ZeroDebugBreak();
}

 void OnTranslationError(TranslationErrorEvent* errorEvent)
{
  String& code = errorEvent->mLocation.Code;
  String msg = errorEvent->GetFormattedMessage(Zilch::MessageFormat::MsvcCpp);
  ZPrint("%s", msg.c_str());
  ZeroDebugBreak();
}


void RealMain()
{
  LibraryBuilder builder{ "" };
  BoundType* classBoundType; 
  StringRange name; 
  StringRange spaceDelimitedNames;


  //Details::SelectOverload<void (*)()>(&Zilch::Console::WriteLine);
  //BoundFn boundFunction = TemplateBinding::FunctionBinding<void (*)()>::template Function<Details::SelectOverload<void (*)()>(&Zilch::Console::WriteLine)>();
  Function* function = TemplateBinding::FunctionBinding<void (*)()>::template MakeFunction<Details::SelectOverload<void (*)()>(&Zilch::Console::WriteLine)>(builder, classBoundType, "", "");

  ZilchSetup mZilchSetup;
  Module mModule;
  ExecutableState::CallingState = mModule.Link();
  
  ShaderSettingsLibrary::InitializeInstance();
  ShaderSettingsLibrary::GetInstance().GetLibrary();
  
  SpirVNameSettings nameSettings;
  SimpleZilchShaderIRGenerator::LoadNameSettings(nameSettings);
  ZilchShaderSpirVSettings* settings = SimpleZilchShaderIRGenerator::CreateZeroSettings(nameSettings);
  auto mGenerator = std::make_unique<SimpleZilchShaderIRGenerator>(new ZilchSpirVFrontEnd(), settings);
  
  
  Zilch::EventConnect(mGenerator.get(), Zilch::Events::CompilationError, &ZilchCompilerErrorCallback);
  Zilch::EventConnect(mGenerator.get(), Zero::Events::TranslationError, &OnTranslationError);
  Zilch::EventConnect(mGenerator.get(), Zilch::Events::CompilationError, &ZilchCompilerErrorCallback);
  
  mGenerator->SetupDependencies("Extensions");
  
  FileRange range("Fragments");
  for (; !range.Empty(); range.PopFront())
  {
    String filePath = range.FrontEntry().GetFullPath();
    String ext = FilePath::GetExtension(filePath);
    if (ext != "zilchFrag" && ext != "zilchfrag")
      continue;
  
    mGenerator->AddFragmentCode(ReadFileIntoString(filePath), filePath, nullptr);
  }
  
  mGenerator->CompileAndTranslateFragments();
  
  Zero::ShaderPipelineDescription* pipeline = new Zero::ShaderPipelineDescription();
  ZilchShaderGlslBackend* glslBackend = new Zero::ZilchShaderGlslBackend();
  glslBackend->mTargetVersion = 440;
  pipeline->mBackend = glslBackend;
  pipeline->mDebugPasses.PushBack(new SpirVValidatorPass());
  pipeline->mDebugPasses.PushBack(new ZilchSpirVDisassemblerBackend());
  
  Array<SimpleZilchShaderIRGenerator::TranslationPassResultRef> pipelineResults;
  Array<SimpleZilchShaderIRGenerator::TranslationPassResultRef> debugResults;
  ZilchShaderIRType* particleComputeType = mGenerator->FindFragmentType("ParticleCompute");
  mGenerator->CompilePipeline(particleComputeType, *pipeline, pipelineResults, debugResults);
  String computeGlsl = pipelineResults.Back()->ToString();
}




#include "Windows.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
  RealMain();
}
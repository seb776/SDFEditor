
#include "Renderer.h"
#include <ThirdParty/glad/glad.h>
#include <GLFW/glfw3.h>

#include "SDFEditor/Utils/ReadFile.h"
#include "SDFEditor/Tool/SceneData.h"

#include "glm/glm.hpp"
#include "glm/mat4x4.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void CRenderer::Init()
{
    glGenVertexArrays(1, &mDummyVAO);
   
    mStrokesBuffer = std::make_shared<CGPUShaderStorageObject>();
}

void CRenderer::Shutdown()
{
    glDeleteVertexArrays(1, &mDummyVAO);
}

void CRenderer::ReloadShaders()
{
    CShaderCodeRef lScreenQuadVSCode = std::make_shared<std::vector<char>>(std::move(ReadFile("./Shaders/FullScreenTrinagle.vert.glsl")));
    CShaderCodeRef lColorFSCode = std::make_shared<std::vector<char>>(std::move(ReadFile("./Shaders/Color.frag.glsl")));

    mFullscreenVertexProgram = std::make_shared<CGPUShaderProgram>(lScreenQuadVSCode, EShaderSourceType::VERTEX_SHADER, "ScreenQuadVS");
    mColorFragmentProgram = std::make_shared<CGPUShaderProgram>(lColorFSCode, EShaderSourceType::FRAGMENT_SHADER, "BaseFragmentFS");

    std::vector<CGPUShaderProgramRef> lPrograms = { mFullscreenVertexProgram, mColorFragmentProgram };

    mScreenQuadPipeline = std::make_shared<CGPUShaderPipeline>(lPrograms);

    mStrokesBuffer->BindToProgram(mColorFragmentProgram->GetHandler(), 0, "strokes_buffer");
}

void CRenderer::UpdateSceneData(CScene const& aScene)
{
    size_t lSizeBytes = aScene.mStorkesArray.size() * sizeof(stroke_t);
    
    if (lSizeBytes > mStrokesBuffer->GetStorageSize())
    {
        mStrokesBuffer->SetData(lSizeBytes, (void*)aScene.mStorkesArray.data(), EGPUBufferFlags::MAP_WRITE_BIT | EGPUBufferFlags::DYNAMIC_STORAGE);
    }
    else
    {
        void* lStrokesBufferMappedMemory = mStrokesBuffer->Map();
        ::memcpy(lStrokesBufferMappedMemory, (void*)aScene.mStorkesArray.data(), lSizeBytes);
        mStrokesBuffer->Unmap();
    }  

    glProgramUniform1ui(mColorFragmentProgram->GetHandler(), 1, aScene.mStorkesArray.size() & 0xFFFFFFFF);
}

void CRenderer::RenderFrame()
{
    glfwGetFramebufferSize(glfwGetCurrentContext(), &mViewWidth, &mViewHeight);

    glViewport(0, 0, mViewWidth, mViewHeight);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);

    //Update Matrix
    glm::mat4 lProjection = glm::perspective(glm::radians(45.0f), (float)mViewWidth / (float)mViewHeight, 0.1f, 100.0f);
    glm::mat4 lView = glm::lookAt(glm::vec3(0.0f, -1.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //lProjection[1][1] *= -1; // Invert glm convention for y-up, as in Vulkan is the opposite than in OpenGL
    glm::mat4 lViewProjection = lProjection * lView;
    glm::mat4 lInverseViewProjection = glm::inverse(lViewProjection);

    // Update program data
    glProgramUniformMatrix4fv(mFullscreenVertexProgram->GetHandler(), 0, 1, false, glm::value_ptr(lInverseViewProjection));

    //Draw full screen quad
    glBindVertexArray(mDummyVAO);
    mScreenQuadPipeline->Bind();

    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    glBindVertexArray(0);
}
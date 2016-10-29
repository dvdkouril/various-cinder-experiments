#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/CameraUi.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class negativespaceApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
private:
    CameraPersp                 mCamera;
    CameraUi                    mCameraUi;
    gl::GlslProgRef             mRenderShader;
    gl::GlslProgRef             mNegativePPShader;
    gl::FboRef                  mRenderTargetFbo;
    gl::Texture2dRef            mTargetTexture;
    gl::BatchRef                mRectPostProcess;
    
    
    gl::GlslProgRef loadShaders(std::string vsFilename, std::string fsFilename);
};

void negativespaceApp::setup()
{
    mCamera.lookAt(vec3(0, 0, -15), vec3(0,0,0));
    mCameraUi = CameraUi(&mCamera, getWindow(), -1);
    
    mRenderShader = loadShaders("render.vs", "render.fs");
    mNegativePPShader = loadShaders("negative.vs", "negative.fs");
    
    gl::VboMeshRef rect             = gl::VboMesh::create( geom::Rect() );
    mRectPostProcess                = gl::Batch::create( rect, mNegativePPShader );
    
    vec2 size = getWindowSize();
    
    mTargetTexture = gl::Texture2d::create(size.x, size.y, gl::Texture2d::Format()
                                           .internalFormat(GL_RGBA16F)
                                           .magFilter(GL_NEAREST)
                                           .minFilter(GL_NEAREST)
                                           .wrap(GL_CLAMP_TO_EDGE)
                                           .dataType(GL_FLOAT));
    
    gl::Fbo::Format fboFormat;
    fboFormat.disableDepth();
    fboFormat.attachment(GL_COLOR_ATTACHMENT0, mTargetTexture);
    mRenderTargetFbo = gl::Fbo::create(size.x, size.y, fboFormat);
    {
        const gl::ScopedFramebuffer sf(mRenderTargetFbo);
        const gl::ScopedViewport sv(ivec2(0), mRenderTargetFbo->getSize());
        gl::clear(Color::white());
    }
    
    // uniform(s)
    mNegativePPShader->uniform("tex", 0);
}

void negativespaceApp::mouseDown( MouseEvent event )
{
}

void negativespaceApp::update()
{
}

void negativespaceApp::draw()
{
    const float PI = 3.14159265358979323846;
    {
        gl::ScopedFramebuffer sf(mRenderTargetFbo);
        gl::drawBuffer(GL_COLOR_ATTACHMENT0);
        const gl::ScopedViewport sV( ivec2(0), mRenderTargetFbo->getSize() );
        gl::clear( Color( 1, 1, 1 ) );
        gl::ScopedGlslProg pass(mRenderShader);
        gl::setMatrices(mCamera);
        gl::pushMatrices();
            gl::translate(vec3(3 * cos(getElapsedFrames() / 60.0), 0, 0));
            gl::rotate(getElapsedFrames() * 0.01, vec3(1,0,0));
            gl::rotate(getElapsedFrames() * 0.01, vec3(0,0,1));
            gl::drawCube(vec3(0), vec3(1.0));
        gl::popMatrices();
    }
    
    {
        const gl::ScopedViewport sv( ivec2(0), toPixels(getWindowSize()));
        const gl::ScopedMatrices sm;
        gl::setMatricesWindow( toPixels( getWindowSize() ) );
        gl::clear(Color::white());
        gl::translate( toPixels( getWindowSize() / 2 )  );
        gl::scale( toPixels( getWindowSize() ) );
        gl::disableDepthRead();
        gl::disableDepthWrite();
        
        const gl::ScopedTextureBind stb(mTargetTexture, 0);
        mRectPostProcess->draw();
    }
    
}

gl::GlslProgRef negativespaceApp::loadShaders(std::string vsFilename, std::string fsFilename) {
    // Shortcut for shader loading and error handling
    auto loadGlslProg = [ & ]( const gl::GlslProg::Format& format ) -> gl::GlslProgRef
    {
        string names = format.getVertexPath().string() + " + " +
        format.getFragmentPath().string();
        gl::GlslProgRef glslProg;
        try {
            glslProg = gl::GlslProg::create( format );
        } catch ( const Exception& ex ) {
            CI_LOG_EXCEPTION( names, ex );
            quit();
        }
        return glslProg;
    };
    
    DataSourceRef vertShader = loadAsset(vsFilename);
    DataSourceRef fragShader = loadAsset(fsFilename);
    
    gl::GlslProgRef shaderProgram = loadGlslProg(gl::GlslProg::Format().version(330)
                                                 .vertex(vertShader).fragment(fragShader));
    
    CI_LOG_D("All shaders loaded and compiled.");
    
    return shaderProgram;
    
}

//CINDER_APP( negativespaceApp, RendererGl )
CINDER_APP( negativespaceApp, RendererGl,
           [](App::Settings* settings)
           {
               settings->setWindowSize(480, 480);
           })

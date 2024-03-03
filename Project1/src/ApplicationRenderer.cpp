#include"ApplicationRenderer.h"


ApplicationRenderer::ApplicationRenderer()
{
}

ApplicationRenderer::~ApplicationRenderer()
{
}



void ApplicationRenderer::WindowInitialize(int width, int height,  std::string windowName)
{
    windowWidth = width;
    WindowHeight = height;
    lastX = windowWidth / 2.0f;
    lastY= WindowHeight / 2.0f;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(width, height, windowName.c_str(), NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* w, int x, int y)
        {
            static_cast<ApplicationRenderer*>(glfwGetWindowUserPointer(w))->SetViewPort(w, x, y);
        });

    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            static_cast<ApplicationRenderer*>(glfwGetWindowUserPointer(window))->KeyCallBack(window, key, scancode, action, mods);
        });


    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xposIn, double yposIn)
        {
            static_cast<ApplicationRenderer*>(glfwGetWindowUserPointer(window))->MouseCallBack(window, xposIn, yposIn);
        });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
        {
            static_cast<ApplicationRenderer*>(glfwGetWindowUserPointer(window))->MouseScroll(window, xoffset, yoffset);
        });
   
   

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.FontGlobalScale = 2.0f;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(this->window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    //Init GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }


   

  
    defaultShader = new Shader("Shaders/DefaultShader_Vertex.vert", "Shaders/DefaultShader_Fragment.frag");
    SolidColorShader = new Shader("Shaders/SolidColor_Vertex.vert", "Shaders/SolidColor_Fragment.frag", SOLID);
    StencilShader = new Shader("Shaders/StencilOutline.vert", "Shaders/StencilOutline.frag", OPAQUE);
   
    alphaBlendShader = new Shader("Shaders/DefaultShader_Vertex.vert", "Shaders/DefaultShader_Fragment.frag", ALPHA_BLEND);
    alphaBlendShader->blendMode = ALPHA_BLEND;

    alphaCutoutShader = new Shader("Shaders/DefaultShader_Vertex.vert", "Shaders/DefaultShader_Fragment.frag", ALPHA_CUTOUT);
    alphaCutoutShader->blendMode = ALPHA_CUTOUT;

    SkyboxShader = new Shader("Shaders/SkyboxShader.vert", "Shaders/SkyboxShader.frag");
    SkyboxShader->modelUniform = false;

    Model* skyBoxMod = new Model("Models/DefaultCube/DefaultCube.fbx",false);

    skyBoxMod->meshes[0]->meshMaterial = new SkyboxMaterial();

    SkyboxMaterial* _skyBoxMaterial = skyBoxMod->meshes[0]->meshMaterial->skyboxMaterial();

    std::vector<std::string> faces
    {
       ("Textures/skybox/right.jpg"),
       ("Textures/skybox/left.jpg"),
       ("Textures/skybox/top.jpg"),
       ("Textures/skybox/bottom.jpg"),
       ("Textures/skybox/front.jpg"),
       ("Textures/skybox/back.jpg")
    };
    _skyBoxMaterial->skyBoxTexture->LoadTexture(faces);

    render.SkyBoxModel = skyBoxMod;
   // render.AddModelsAndShader(render.SkyBoxModel, SkyboxShader);

    //ScrollShader = new Shader("Shaders/ScrollTexture.vert", "Shaders/ScrollTexture.frag");
    render.AssignStencilShader(StencilShader);


    camera.transform.position = glm::vec3(0, 0, - 1.0f);
}



void ApplicationRenderer::Start()
{
 //   GLCALL(glEnable(GL_DEPTH_TEST));
    GLCALL(glDepthFunc(GL_LESS));
    GLCALL(glEnable(GL_STENCIL_TEST));
    GLCALL(glStencilFunc(GL_NOTEQUAL, 1, 0xFF));
    GLCALL(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
    GLCALL(glEnable(GL_BLEND));
    GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));


    

    render.AssignCamera(&camera);

    Model* Sphere = new Model((char*)"Models/DefaultSphere/Sphere_1_unit_Radius.ply", true);

     Model* dir = new Model("Models/DefaultSphere/Sphere_1_unit_Radius.ply",false);
     dir->isVisible = false;
     dir->transform.SetScale(glm::vec3(0.5f));


     Light directionLight;
     directionLight.lightType = LightType::DIRECTION_LIGHT;
     directionLight.lightModel = dir;
     directionLight.ambient =  glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
   //  directionLight.diffuse =  glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
    // directionLight.specular = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
     directionLight.color = glm::vec4(1, 1, 1, 1.0f);
     directionLight.linear = 1;
     directionLight.constant = 1;
     directionLight.quadratic = 0.01f;
     directionLight.cutOffAngle = 11;
     directionLight.outerCutOffAngle = 12;
     dir->transform.SetRotation(glm::vec3(0, 0, 0));
     dir->transform.SetPosition(glm::vec3(0, 0, 2));

     render.AddModelAndShader(dir,SolidColorShader);


     //LightRenderer
     lightManager.AddNewLight(directionLight);

     

     camera.transform.SetPosition(glm::vec3(0, 0, -20));

     AnimationScene();
}

void ApplicationRenderer::PreRender()
{
    glm::mat4 _projection = glm::perspective(glm::radians(camera.Zoom), (float)windowWidth / (float)WindowHeight, 0.1f, 100.0f);
    glm::mat4 _view = camera.GetViewMatrix();
    glm::mat4 _skyboxview = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    glDepthFunc(GL_LEQUAL);
    SkyboxShader->Bind();
    SkyboxShader->setMat4("view", _skyboxview);
    SkyboxShader->setMat4("projection", _projection);

    render.SkyBoxModel->Draw(*SkyboxShader);
    glDepthFunc(GL_LESS);


    defaultShader->Bind();
    // material.SetMaterialProperties(*defaultShader);
    lightManager.UpdateUniformValuesToShader(defaultShader);
    //  lightManager.UpdateUniformValues(defaultShader->ID);


    defaultShader->setMat4("projection", _projection);
    defaultShader->setMat4("view", _view);
    defaultShader->setVec3("viewPos", camera.transform.position.x, camera.transform.position.y, camera.transform.position.z);
    defaultShader->setFloat("time", scrollTime);
    defaultShader->setBool("isDepthBuffer", false);

    alphaBlendShader->Bind();
    lightManager.UpdateUniformValuesToShader(alphaBlendShader);
    alphaBlendShader->setMat4("projection", _projection);
    alphaBlendShader->setMat4("view", _view);
    alphaBlendShader->setVec3("viewPos", camera.transform.position.x, camera.transform.position.y, camera.transform.position.z);
    alphaBlendShader->setFloat("time", scrollTime);
    alphaBlendShader->setBool("isDepthBuffer", false);

    alphaCutoutShader->Bind();
    lightManager.UpdateUniformValuesToShader(alphaCutoutShader);
    alphaCutoutShader->setMat4("projection", _projection);
    alphaCutoutShader->setMat4("view", _view);
    alphaCutoutShader->setVec3("viewPos", camera.transform.position.x, camera.transform.position.y, camera.transform.position.z);
    alphaCutoutShader->setFloat("time", scrollTime);
    alphaCutoutShader->setBool("isDepthBuffer", false);

    SolidColorShader->Bind();
    SolidColorShader->setMat4("projection", _projection);
    SolidColorShader->setMat4("view", _view);

    StencilShader->Bind();
    StencilShader->setMat4("projection", _projection);
    StencilShader->setMat4("view", _view);

    /* ScrollShader->Bind();
       ScrollShader->setMat4("ProjectionMatrix", _projection);*/

    imguiRender = true;
}

void ApplicationRenderer::Render()
{
   
    Start();
  
    EditorLayout::GetInstance().InitializeEditors();

    Time::GetInstance().lastFrame = glfwGetTime();
   // glEnable(GL_BLEND);
  //  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
    while (!glfwWindowShouldClose(window))
    {

        Time::GetInstance().SetCurrentTime(glfwGetTime());
       
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        scrollTime += deltaTime;

        ProcessInput(window);

        // Imgui

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        PanelManager::GetInstance().Update((float)windowWidth, (float)WindowHeight);

        ImGui::Render();

        Clear();

        PreRender(); //Update call BEFORE  DRAW
        
         // make models that it should not write in the stencil buffer
         render.Draw();

         EntityManager::GetInstance().Update(Time::GetInstance().deltaTime);

         PostRender(); // Update Call AFTER  DRAW

         ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void ApplicationRenderer::PostRender()
{


    m_FrameNumber++;
    if (m_FrameNumber > 1000) m_FrameNumber = 0;
    float val = (float)m_FrameNumber / 250.f;


   
      //  AnimationSystem::GetInstance().Update(Time::GetInstance().deltaTime);
        AnimationSystem::GetInstance().UpdateAnimationSequence(Time::GetInstance().deltaTime);
   
}


void ApplicationRenderer::Clear()
{
    GLCALL(glClearColor(0.1f, 0.1f, 0.1f, 0.1f));
    GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
}

void ApplicationRenderer::ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed=25;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime * cameraSpeed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime * cameraSpeed);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime * cameraSpeed);

    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime * cameraSpeed);

    }


}

void ApplicationRenderer::AnimationScene()
{
#pragma region Animation StartUps


    #pragma region Sequence1


    Model* SpaceShip = new Model("Models/SpaceShip/SpaceShip.ply");

    SpaceShip->name = "SPACESHIP";
    SpaceShip->entityID = "SPACESHIP_1";
    SpaceShip->transform.SetPosition(glm::vec3(-15, 0, 0));
    SpaceShip->transform.SetScale(glm::vec3(0.02f));

    render.AddModelAndShader(SpaceShip, defaultShader);



    Animation* clip1 = new Animation();
    clip1->AddPositionKeyFrame(glm::vec3(-15, 0, 0), 0);
    clip1->AddPositionKeyFrame(glm::vec3(-10, 2, 0), 1, EasingType::sineEaseIn);
    clip1->AddPositionKeyFrame(glm::vec3(-5, -2, -5), 2, EasingType::sineEaseIn);
    clip1->AddPositionKeyFrame(glm::vec3(2, -2, -5), 3, EasingType::sineEaseIn);
    clip1->AddPositionKeyFrame(glm::vec3(12, 0, 0), 4, EasingType::sineEaseIn);
    clip1->AddPositionKeyFrame(glm::vec3(20, 0, 0), 5, EasingType::sineEaseIn);

    clip1->AddRotationKeyFrame(glm::vec3(0, 0, 0), 0 );
    clip1->AddRotationKeyFrame(glm::vec3(-90, 0, 0), 2);
    clip1->AddRotationKeyFrame(glm::vec3(-45, 0, 0), 3);
    clip1->AddRotationKeyFrame(glm::vec3(0, 0, 0), 5);

    clip1->AddColoreKeyFrame(glm::vec3(1, 0, 0), 0, EasingType::sineEaseIn);



    Model* asteroid = new Model("Models/Asteroids/Asteroid_3.ply");

    asteroid->name = "ASTEROID";
    asteroid->entityID = "ASTEROID_1";

    asteroid->transform.SetPosition(glm::vec3(15, 0, 0));
    asteroid->transform.SetScale(glm::vec3(0.018f));

    render.AddModelAndShader(asteroid, defaultShader);

    Animation* clip2 = new Animation();

    clip2->AddPositionKeyFrame(glm::vec3(15, 0, 0), 0);
    clip2->AddPositionKeyFrame(glm::vec3(10, 0, 0), 1);
    clip2->AddPositionKeyFrame(glm::vec3(5, 0, 0), 2);
    clip2->AddPositionKeyFrame(glm::vec3(0, 0, 0), 3);
    clip2->AddPositionKeyFrame(glm::vec3(-15, 0, 0), 5);
    clip2->AddPositionKeyFrame(glm::vec3(-50, 0, 0), 6);

    clip2->AddRotationKeyFrame(glm::vec3(0, 0,0 ), 0, EasingType::sineEaseIn);
    clip2->AddRotationKeyFrame(glm::vec3(0, 0,90), 2);
    clip2->AddRotationKeyFrame(glm::vec3(0, 0,180), 3);
    clip2->AddRotationKeyFrame(glm::vec3(0, 0,270), 5);
    clip2->AddRotationKeyFrame(glm::vec3(0, 0,360), 6, EasingType::sineEaseOut);


    Model* asteroidGroups = new Model("Models/Asteroids/AsteroidGroup.ply");

    asteroidGroups->name = "ASTEROID GROUPS";
    asteroidGroups->entityID = "ASTEROID_GROUP_1";

    asteroidGroups->transform.SetPosition(glm::vec3(-20, -3, 15));
    asteroidGroups->transform.SetRotation(glm::vec3(0, 90, 0));

    asteroidGroups->transform.SetScale(glm::vec3(0.0025f));

    render.AddModelAndShader(asteroidGroups, defaultShader);




    Animation* clip3 = new Animation();

    clip3->AddPositionKeyFrame(glm::vec3(-20, -3, 15), 0, EasingType::sineEaseOut);
    clip3->AddPositionKeyFrame(glm::vec3(25, -3, 15), 6, EasingType::sineEaseOut);

    clip3->AddColoreKeyFrame(glm::vec3(1, 1, 0), 0);


    AnimationSequence* sequence1 = new AnimationSequence();
    sequence1->AddAnimationClip(clip1, SpaceShip);
    sequence1->AddAnimationClip(clip2, asteroid);
    sequence1->AddAnimationClip(clip3, asteroidGroups);

    AnimationSystem::GetInstance().SetSequence(sequence1);

#pragma endregion

    #pragma region Sequence 2









    Model* fighterJet = new Model();
    fighterJet->LoadModel("Models/FighterJet/FighterJet.ply",false);

    fighterJet->name = "FIGHTERJET";
    fighterJet->entityID = "FIGHTER_JET_1";

    fighterJet->transform.SetPosition(glm::vec3(0, 12, 0));
    fighterJet->transform.SetScale(glm::vec3(0.025f));

    render.AddModelAndShader(fighterJet, SolidColorShader);

    Model* asteroid2 = new Model(*asteroid);

    asteroid2->name = "ASTEROID";
    asteroid2->entityID = "ASTEROID_2";

    asteroid2->transform.SetPosition(glm::vec3(0, 0, -35));
    asteroid2->transform.SetScale(glm::vec3(0.018f));

    render.AddModelAndShader(asteroid2, defaultShader);


    Animation* clip4 = new Animation();
     clip4->AddPositionKeyFrame(glm::vec3(0, 0, -35), 0,EasingType::sineEaseInOut);
     clip4->AddPositionKeyFrame(glm::vec3(0, 0, 20), 5, EasingType::sineEaseInOut);

     clip4->AddRotationKeyFrame(glm::vec3(0, 0,0 ), 0, EasingType::sineEaseInOut);
     clip4->AddRotationKeyFrame(glm::vec3(0, 0,90), 2);
     clip4->AddRotationKeyFrame(glm::vec3(360, 0,180), 3);
     clip4->AddRotationKeyFrame(glm::vec3(0, 0,270), 5);
     clip4->AddRotationKeyFrame(glm::vec3(360, 0,360), 6, EasingType::sineEaseInOut);

     clip4->AddColoreKeyFrame(glm::vec3(0, 1, 0), 0);



     Animation* clip5 = new Animation();

     clip5->AddPositionKeyFrame(glm::vec3(0, 10, 0), 0, EasingType::sineEaseInOut);
     clip5->AddPositionKeyFrame(glm::vec3(0, 7, 0), 2);
     clip5->AddPositionKeyFrame(glm::vec3(0, 3, 0), 4);
     clip5->AddPositionKeyFrame(glm::vec3(0, 2, 0), 5, EasingType::sineEaseInOut);

     clip5->AddRotationKeyFrame(glm::vec3(0, 0, 0), 0, EasingType::sineEaseIn);
     clip5->AddRotationKeyFrame(glm::vec3(0, 0, 0), 0, EasingType::sineEaseIn);
     clip5->AddRotationKeyFrame(glm::vec3(5, 2, 0), 0, EasingType::sineEaseIn);
     clip5->AddRotationKeyFrame(glm::vec3(-20, 5, 0), 5, EasingType::sineEaseInOut);

     clip5->AddScaleKeyFrame(glm::vec3(0.018f), 0, EasingType::sineEaseIn);
     clip5->AddScaleKeyFrame(glm::vec3(0.018f), 2, EasingType::sineEaseIn);
     clip5->AddScaleKeyFrame(glm::vec3(0.018f), 3, EasingType::sineEaseInOut);
     clip5->AddScaleKeyFrame(glm::vec3(0), 3.1, EasingType::sineEaseInOut);
     clip5->AddScaleKeyFrame(glm::vec3(0), 5, EasingType::sineEaseInOut);
     


     clip5->AddColoreKeyFrame(glm::vec3(0,1,0), 0, EasingType::sineEaseInOut);
     clip5->AddColoreKeyFrame(glm::vec3(1,1,1), 2);
     clip5->AddColoreKeyFrame(glm::vec3(1,1,1), 4);
     clip5->AddColoreKeyFrame(glm::vec3(0,1,0), 5, EasingType::sineEaseInOut);


     Model* explosion = new Model();
     
     explosion->LoadModel("Models/DefaultSphere/Sphere_1_unit_Radius.ply", false);

     explosion->name = "EXLPOSION";
     explosion->entityID = "EXPLOSION_1";

     explosion->transform.SetPosition(glm::vec3(0, 5, 0));
     explosion->transform.SetScale(glm::vec3(0));

     render.AddModelAndShader(explosion, SolidColorShader);


     Animation* clip6 = new Animation();

     clip6->AddScaleKeyFrame(glm::vec3(0), 0);
     clip6->AddScaleKeyFrame(glm::vec3(0), 1);
     clip6->AddScaleKeyFrame(glm::vec3(0), 2);
     clip6->AddScaleKeyFrame(glm::vec3(0), 3);
     clip6->AddScaleKeyFrame(glm::vec3(5), 4);
     clip6->AddScaleKeyFrame(glm::vec3(3), 4.1);
     clip6->AddScaleKeyFrame(glm::vec3(2), 4.5);
     clip6->AddScaleKeyFrame(glm::vec3(0), 5);

     clip6->AddColoreKeyFrame(glm::vec3(1,0.5f,0), 0, EasingType::sineEaseInOut);
     clip6->AddColoreKeyFrame(glm::vec3(1,0.5f,0), 2);
     clip6->AddColoreKeyFrame(glm::vec3(1,0.5f,0), 4);
     clip6->AddColoreKeyFrame(glm::vec3(1,0.5f,0), 5, EasingType::sineEaseInOut);

     clip6->AddEventKeyFrame(3, []
         {
             std::cout << "##### EXPLODED! #####";
         });

     AnimationSequence* sequence2 = new AnimationSequence();

     sequence2->AddAnimationClip(clip4, asteroid2);
     sequence2->AddAnimationClip(clip5, fighterJet);
     sequence2->AddAnimationClip(clip6, explosion);

   //  AnimationSystem::GetInstance().SetSequence(sequence2);
#pragma endregion


    #pragma region Sequence 3

   

     SpaceShip->transform.SetPosition(glm::vec3(-15, 0, 0));
     SpaceShip->transform.SetScale(glm::vec3(0.02f));


     Animation* clip7 = new Animation();

     clip7->AddPositionKeyFrame(glm::vec3(20, 0, 0), 0);
     clip7->AddPositionKeyFrame(glm::vec3(-15, 0, 0), 2);
     clip7->AddPositionKeyFrame(glm::vec3(-30, 0, 0), 4);


     clip7->AddRotationKeyFrame(glm::vec3(0, -180, 0), 0);
     clip7->AddRotationKeyFrame(glm::vec3(0, -180, 0), 2);
     clip7->AddRotationKeyFrame(glm::vec3(0, -180, 0), 3);
     clip7->AddRotationKeyFrame(glm::vec3(0, -180, 0), 4);


     Animation* clip8 = new Animation();

     clip8->AddPositionKeyFrame(glm::vec3(20, 0, 0),0,EasingType::sineEaseOut);
     clip8->AddPositionKeyFrame(glm::vec3(20, 0, 0),1);
     clip8->AddPositionKeyFrame(glm::vec3(-15, 0, 0),4, EasingType::sineEaseOut);


     clip8->AddRotationKeyFrame(glm::vec3(0, -90, 0), 0);
     clip8->AddRotationKeyFrame(glm::vec3(0, -90, 0), 1);
    // clip8->AddRotationKeyFrame(glm::vec3(0, -90, -90), 1);
     clip8->AddRotationKeyFrame(glm::vec3(0, -90, -180), 2);
     clip8->AddRotationKeyFrame(glm::vec3(0, -90,-270 ), 3);
     clip8->AddRotationKeyFrame(glm::vec3(0, -90, -359), 4);

     clip8->AddScaleKeyFrame(glm::vec3(0.02f), 0);
     clip8->AddScaleKeyFrame(glm::vec3(0.01f), 3,EasingType::sineEaseInOut);
     clip8->AddScaleKeyFrame(glm::vec3(0.02f), 4, EasingType::sineEaseInOut);

     clip8->AddColoreKeyFrame(glm::vec3(1, 1, 0), 0, EasingType::sineEaseOut);
     clip8->AddColoreKeyFrame(glm::vec3(1, 1, 1), 1);
     clip8->AddColoreKeyFrame(glm::vec3(1, 1, 0), 4, EasingType::sineEaseOut);



     Animation* clip9 = new Animation();

     clip9->AddPositionKeyFrame(glm::vec3(30, 0, 5), 0,EasingType::sineEaseIn);
     clip9->AddPositionKeyFrame(glm::vec3(5, 0, 5), 1);
     clip9->AddPositionKeyFrame(glm::vec3(-5, 0, 5), 2);
     clip9->AddPositionKeyFrame(glm::vec3(-30, 0, 5), 4, EasingType::sineEaseInOut);


     clip9->AddRotationKeyFrame(glm::vec3(0, 90, 0), 0, EasingType::sineEaseIn);
     clip9->AddRotationKeyFrame(glm::vec3(5, 90, 0), 1);
     clip9->AddRotationKeyFrame(glm::vec3(0, 90, 0), 2);
     clip9->AddRotationKeyFrame(glm::vec3(5, 90, 0), 3, EasingType::sineEaseInOut);
     clip9->AddRotationKeyFrame(glm::vec3(0, 90, 0), 4, EasingType::sineEaseInOut);


     clip9->AddColoreKeyFrame(glm::vec3(1, 0, 0), 0, EasingType::sineEaseIn);
     clip9->AddColoreKeyFrame(glm::vec3(1, 1, 1), 1);
     clip9->AddColoreKeyFrame(glm::vec3(1, 1,1), 2);
     clip9->AddColoreKeyFrame(glm::vec3(0, 1, 0), 4, EasingType::sineEaseOut);

 


     AnimationSequence* sequence3 = new AnimationSequence();


     sequence3->AddAnimationClip(clip7, SpaceShip);
     sequence3->AddAnimationClip(clip8, fighterJet);
     sequence3->AddAnimationClip(clip9, asteroidGroups);


   //  AnimationSystem::GetInstance().SetSequence(sequence3);

    #pragma endregion



#pragma endregion
}

 void ApplicationRenderer::SetViewPort(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

 void ApplicationRenderer::KeyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
 {  
         if (key == GLFW_KEY_V && action == GLFW_PRESS)
         {

            
             std::cout << "V pressed" << std::endl;

             std::vector<Model*> listOfModels = render.GetModelList();
            
          

             selectedModelCount++;

             if (selectedModelCount > listOfModels.size()-1)
             {
                 selectedModelCount = 0;
             }

            
             render.selectedModel = listOfModels[selectedModelCount];


         }
     
         if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
         {
             AnimationSystem::GetInstance().TogglePlayOrPause();
         }
         if (key == GLFW_KEY_1 && action == GLFW_PRESS)
         {
             AnimationSystem::GetInstance().SetAnimationSpeed(1);
         }
         if (key == GLFW_KEY_2 && action == GLFW_PRESS)
         {
             AnimationSystem::GetInstance().SetAnimationSpeed(2);

         }
         if (key == GLFW_KEY_3 && action == GLFW_PRESS)
         {
             AnimationSystem::GetInstance().SetAnimationSpeed(3);

         }
         if (key == GLFW_KEY_4 && action == GLFW_PRESS)
         {
             AnimationSystem::GetInstance().SetAnimationSpeed(4);

         }
         if (key == GLFW_KEY_5 && action == GLFW_PRESS)
         {
             AnimationSystem::GetInstance().SetAnimationSpeed(5);

         }

         if (key == GLFW_KEY_R && action == GLFW_PRESS)
         {
             AnimationSystem::GetInstance().ToggleRewind();

         }

         if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
         {
             AnimationSystem::GetInstance().NextSequence();

         }
         if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
         {
             AnimationSystem::GetInstance().PreviousSequence();

         }
 }

 void ApplicationRenderer::MouseCallBack(GLFWwindow* window, double xposIn, double yposIn)
 {

    float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);
     
        if (firstMouse)
        {

        }

         if (firstMouse)
         {
             lastX = xpos;
             lastY = ypos;
             firstMouse = false;
         }
     
         float xoffset = xpos - lastX;
         float yoffset = lastY - ypos;
     
         lastX = xpos;
         lastY = ypos;
     
         if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
         {
             camera.ProcessMouseMovement(xoffset, yoffset);
         }
 }

 void ApplicationRenderer::MouseScroll(GLFWwindow* window, double xoffset, double yoffset)
 {
     camera.ProcessMouseScroll(static_cast<float>(yoffset));
 }

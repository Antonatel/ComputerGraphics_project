#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>
#include<cmath>
#include<time.h>


#define PI 3.1415926535898

struct particles
{
	glm::mat4 modelposition;
	int type;
	float position_x, position_y, position_z, vx, vy, vz, acc_x, acc_y, acc_z;
	
};

particles particle1[20];



float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadCubemap(vector<std::string> faces);
unsigned int loadTexture(char const * path);



const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;


Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool isOn = false;
bool showBomb = false;
bool start = false;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
glm::float32 i = -3.0f;


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	glEnable(GL_DEPTH_TEST);

	Shader basicshader("light1.vs","light1.fs");
	Shader roomshader("flashlight.vs", "flashlight.fs");
	Shader budahashaer("budaha.vs","budaha.fs");
	Shader shader("bump_mapping.vs", "bump_mapping.fs");
	Shader skyboxShader("skybox.vs", "skybox.fs");
	
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	vector<std::string> faces
	{
		"cave/criminal-intentions_ft.tga",
		"cave/criminal-intentions_bk.tga",
		"cave/criminal-intentions_up.tga",
		"cave/criminal-intentions_dn.tga",
		"cave/criminal-intentions_rt.tga",
		"cave/criminal-intentions_lf.tga",
	};
	unsigned int cubemapTexture = loadCubemap(faces);
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);



	GLuint diffuseMap = loadTexture((char*)"book/texture_0_6195987693227547908.png");
	GLuint normalMap = loadTexture((char*)"book/texture_0_6195987693227547908_NRM.jpg");

	GLuint rockdiffuse = loadTexture((char*)"rocks/newrock/texture_0_6195987693227547908.png");
	GLuint rockspecular = loadTexture((char*)"rocks/newrock/texture_0_6195987693227547908_SPEC.jpg");

	GLuint roomdiffuse = loadTexture((char*)"room/newroom/newtest/texture_0_6195987693227547908.png");
	GLuint roomspecular = loadTexture((char*)"room/newroom/newtest/texture_0_6195987693227547908_SPEC.jpg");

	GLuint bombdiffuse = loadTexture((char*)"bomb/newbomb/texture_2_11563080508590297124.png");
	GLuint bombspecular = loadTexture((char*)"bomb/newbomb/texture_2_11563080508590297124_SPEC.jpg");

	shader.use();
	shader.setInt("diffuseMap", 0);
	shader.setInt("normalMap", 1);

	basicshader.use();
	basicshader.setInt("material.diffuse", 6);
	basicshader.setInt("material.specular", 7);


	glm::vec3 lightPos(0.5f, 0.0f, 0.3f);
	Model bomb("bomb/newbomb/newbomb.obj");
	Model bigrock("rocks/newrock/bigrock.obj");
	Model bookmodel("book/book4.obj");
	Model ourModel("test1/bookhalf1.obj");
	Model ourModel2("test2/bookhalf2.obj");
	Model room("room/newroom/newtest/newroom.obj");
	Model rock1("rocks/rock1/rock1.obj");
	Model rock2("rocks/rock2/rock2.obj");
	Model buddha4("happy_recon/test1/happy4.obj");
	Model buddha3("happy_recon/test1/happy3.obj");
	Model buddha2("happy_recon/test1/happy2.obj");
	Model buddha1("happy_recon/test1/happy1.obj");

	bool first = true;
	bool begin = false;
	bool boom = false;

	int step = 0;
	float i = 0.0f;
	float s = 0.0f;
	float r = 0.0f;
	float p = 1.0f;
	float ii = 0.0f;
	float rono = 0.0f;
	int level = 1;
	glm::vec3 fire = glm::vec3(0.2f,0.0f,0.0f);
	glm::vec3 inipos = glm::vec3(0.0f, 0.0f, -3.5f);
	glm::vec3 mc(0.05f,0.05f,0.05f);
	glm::vec3 busp(0.0f,0.0f,0.0f);

	srand((int)time(0));
	for (int tmp = 0; tmp < 20; ++tmp)
	{
		particle1[tmp].type = rand() % 100 ;
		particle1[tmp].position_x = inipos.x;
		particle1[tmp].position_y = inipos.y;
		particle1[tmp].position_z = inipos.z;
		particle1[tmp].acc_x = particle1[tmp].acc_z = 0.0f;
		particle1[tmp].acc_y = -0.0002f;
		particle1[tmp].vx = (double)(rand() % 100) / 1000 - 0.03;
		particle1[tmp].vz = (double)(rand() % 100) / 1000 - 0.05;
		particle1[tmp].vy = (double)(rand() % 100) / 1000- 0.02;
	}
	
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();


		switch (step)
		{
		case 0:
		{
			if (!start)
				break;
			if (i <= 3.2f && begin == false) {
				i += 0.01f;
				lightPos.x = i;
				if (i >= 3.19f)
					begin = true;
			}
			if (begin) {
				s += 0.01f;
				lightPos.x = s;
				if (s >= 3.0f) {
					step = 1;
					s = i = 0.0f;
				}
			}
			shader.use();
			glm::mat4 projection1 = glm::perspective(camera.Zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			shader.setMat4("view", view);
			shader.setMat4("projection", projection1);
			glm::mat4 model;
			model = glm::scale(model, glm::vec3(0.013f, 0.013f, 0.013f));
			model = glm::translate(model, glm::vec3(25.0f, 25.0f, 0.0f));
			model = glm::rotate(model, glm::radians(-5.0f), glm::normalize(glm::vec3(0.0, 0.0, 1.0)));
			model = glm::rotate(model, glm::radians(25.0f), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
			model = glm::rotate(model, glm::radians(0.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
			model = glm::translate(model, glm::vec3(150 - 50.0*i, 0.0f, 10.0f));
			model = glm::rotate(model, i * -4, glm::normalize(glm::vec3(0.0, 1.0, 0.0))); 

			shader.setMat4("model", model);
			shader.setVec3("viewPos", camera.Position);
			shader.setVec3("lightPos", lightPos);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, normalMap);

			bookmodel.Draw(shader);
			break;
		}
		case 1:
		{
			shader.use();
			shader.setMat4("projection", projection);
			shader.setMat4("view", view);
			if (i <= -130) {
				s += 0.001f;
				camera.ProcessKeyboard(FORWARD, deltaTime / 10);
				camera.ProcessMouseMovement(-0.2, 0);
				if (s >= 1.0f) {
					step = 2;
					s == 1.0f;
					i = 0.0f;
					
				}
			}
			shader.setVec4("fogcolor", glm::vec4(s, s, s, 0));
			glm::mat4 model1 = glm::mat4(1.0f);
			glm::mat4 model2 = glm::mat4(1.0f);
			if (i >= -150)
				i -= 0.3f;
			model1 = glm::scale(model1, glm::vec3(0.01f, 0.01f, 0.01f));
			model1 = glm::translate(model1, glm::vec3(-50.42f, -100.25f, -7.05f)); 
			model1 = glm::rotate(model1, glm::radians(150.0f), glm::vec3(0.0, 1.0, 0.0));
			model1 = glm::rotate(model1, glm::radians(i), glm::vec3(0.0, 1.0, 0.0));

			model2 = glm::scale(model2, glm::vec3(0.01f, 0.01f, 0.01f));
			model2 = glm::translate(model2, glm::vec3(-50.42f, -100.25f, -15.55f)); 
			model2 = glm::rotate(model2, glm::radians(150.0f), glm::vec3(0.0, 1.0, 0.0));

			shader.setMat4("model", model1);
			ourModel.Draw(shader);
			shader.setMat4("model", model2);
			ourModel2.Draw(shader);
			
			break;
		}
		case 2: 
		{

			budahashaer.use();
			budahashaer.setVec3("light.position", camera.Position);
			budahashaer.setVec3("light.direction", camera.Front);
			budahashaer.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
			budahashaer.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
			budahashaer.setVec3("viewPos", camera.Position);
			budahashaer.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
			if (isOn) {
				busp.x += 0.0001f;
				busp.y += 0.0001f;
				busp.z += 0.0001f;
				budahashaer.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
				budahashaer.setVec3("light.specular", busp);
			}
			else {
				budahashaer.setVec3("light.diffuse", 0.0f, 0.0f, 0.0f);
				budahashaer.setVec3("light.specular", 0.0f, 0.0f, 0.0f);
			}
			budahashaer.setFloat("light.constant", 1.0f);
			budahashaer.setFloat("light.linear", 0.09f);
			budahashaer.setFloat("light.quadratic", 0.032f);
			budahashaer.setFloat("material.shininess", 128.0f);
			if (mc.x <= 0.95f &&isOn)
				mc.x += 0.0004f;
			if (mc.y <= 0.75f&&isOn)
				mc.y += 0.0004f;
			if (level<=4 &&mc.z<=0.75f&&isOn)
				mc.z += 0.0004f;
			else if(mc.z>=0.32f&&isOn)
				mc.z -= 0.0004f;
			budahashaer.setVec3("materialcolor",mc);

			if (!boom)
				budahashaer.setVec3("fogcolor", glm::vec3(s, s, s));
			else {
				budahashaer.setVec3("fogcolor", glm::vec3(p + 1.0f, p + 1.0f, p));
				p -= 0.1f;
				if (p <= 0)
					budahashaer.setVec3("fogcolor", glm::vec3(0.0f, 0.0f, 0.0f));
			}
			budahashaer.setMat4("projection", projection);
			budahashaer.setMat4("view", view);
			if (boom) {
				glm::mat4 budahamodel = glm::mat4(1.0f);
				budahamodel = glm::translate(budahamodel, glm::vec3(0.0f, -1.0f, -3.5f)); 
				budahamodel = glm::scale(budahamodel, glm::vec3(0.024f, 0.024f, 0.024f));
				budahashaer.setMat4("model", budahamodel);
				ii += 0.002f;
				if (ii >= 1.0f) {
					ii = 0.0f;
					level += 1;
				}
				switch (level)
				{
				case 1:buddha4.Draw(budahashaer); break;
				case 2:buddha3.Draw(budahashaer); break;
				case 3:buddha2.Draw(budahashaer); break;
				case 4:buddha1.Draw(budahashaer); break;
				default:
					buddha1.Draw(budahashaer); break;
					break;
				}
				
			}
			roomshader.use();
			roomshader.setVec3("light.position", camera.Position);
			roomshader.setVec3("light.direction", camera.Front);
			roomshader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
			roomshader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
			roomshader.setVec3("viewPos", camera.Position);
			if (s >= 0.0f)
				s -= 0.005f;

	
			roomshader.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
			
			if (isOn) {
				roomshader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
				roomshader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
			}
			else {
				roomshader.setVec3("light.diffuse", 0.0f, 0.0f, 0.0f);
				roomshader.setVec3("light.specular", 0.0f, 0.0f, 0.0f);
			}

			roomshader.setFloat("light.constant", 1.0f);
			roomshader.setFloat("light.linear", 0.09f);
			roomshader.setFloat("light.quadratic", 0.032f);

		
			roomshader.setFloat("material.shininess", 128.0f);
			if (!boom)
				roomshader.setVec3("fogcolor", glm::vec3(s, s, s));
			else {
				roomshader.setVec3("fogcolor", glm::vec3(p + 1.0f, p + 1.0f, p));
				p -= 0.1f;
				if (p <= 0)
					roomshader.setVec3("fogcolor", glm::vec3(0.0f, 0.0f, 0.0f));
			}
			roomshader.setMat4("projection", projection);
			roomshader.setMat4("view", view);

			if (!boom) {

				glm::mat4 rockmodel = glm::mat4(1.0f);
				rockmodel = glm::translate(rockmodel, glm::vec3(0.0f, -1.0f, -3.5f)); 
				rockmodel = glm::scale(rockmodel, glm::vec3(0.024f, 0.024f, 0.024f));
				rockmodel = glm::rotate(rockmodel, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
				roomshader.setMat4("model", rockmodel);
				roomshader.setInt("material.diffuse", 2);
				roomshader.setInt("material.specular", 3);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, rockdiffuse);
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, rockspecular);
				
				bigrock.Draw(roomshader);
			}

			if (boom)
			{
				//
				rono += 0.005f;
				
				for (int tmp = 0; tmp < 20; ++tmp)
				{
					
					particle1[tmp].position_x += particle1[tmp].vx;
					particle1[tmp].position_y += particle1[tmp].vy;
					particle1[tmp].position_z += particle1[tmp].vz;
					particle1[tmp].vy += particle1[tmp].acc_y;
					if (particle1[tmp].position_y < -1.0f)
					{
						particle1[tmp].position_y = -1.0f;
						particle1[tmp].vx = 0;
						particle1[tmp].vz = 0;
						particle1[tmp].acc_y = 0;
					}
					if (particle1[tmp].position_y > 2.8f) {
						particle1[tmp].position_y = 2.75f;
						particle1[tmp].vy = -particle1[tmp].vy;
					}
					if (particle1[tmp].position_z < -3.5f) {
						particle1[tmp].position_z = -3.45f;
						particle1[tmp].vz = -particle1[tmp].vz;
					}
					particle1[tmp].modelposition = glm::mat4(1.0f);
					particle1[tmp].modelposition = glm::translate(particle1[tmp].modelposition, glm::vec3(particle1[tmp].position_x, particle1[tmp].position_y, particle1[tmp].position_z));
					particle1[tmp].modelposition = glm::scale(particle1[tmp].modelposition, glm::vec3(0.014f, 0.014f, 0.014f));
					particle1[tmp].modelposition = glm::rotate(particle1[tmp].modelposition, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));

					roomshader.setMat4("model", particle1[tmp].modelposition);
					roomshader.setInt("material.diffuse", 2);
					roomshader.setInt("material.specular", 3);

					glActiveTexture(GL_TEXTURE2);
					glBindTexture(GL_TEXTURE_2D, rockdiffuse);
					glActiveTexture(GL_TEXTURE3);
					glBindTexture(GL_TEXTURE_2D, rockspecular);
				
					if (particle1[tmp].type <= 50)
						rock1.Draw(roomshader);
					else
						rock2.Draw(roomshader);
				}
			}
			glm::mat4 roommodel = glm::mat4(1.0f);

			roommodel = glm::translate(roommodel, glm::vec3(-5.0f, -1.0f, 3.0f)); // translate it down so it's at the center of the scene
			roommodel = glm::scale(roommodel, glm::vec3(0.001f, 0.001f, 0.001f));
			roommodel = glm::rotate(roommodel, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
			roommodel = glm::rotate(roommodel, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
			roomshader.setInt("material.diffuse", 4);
			roomshader.setInt("material.specular", 5);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, roomdiffuse);
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, roomspecular);
			roomshader.setMat4("model", roommodel);
			room.Draw(roomshader);

			if (showBomb && !boom)
			{
				if (r <= 4.0f) {
					r += 0.001f;
					if (r >= 0.37f) {
						r = 0.0f;
						boom = true;
						isOn = false;
					}
				}
				fire.z -= 0.05 * r;
				if (r <= 0.25f) {
					fire.y += 0.02*r;
				}
				else {
					fire.y -= 0.02*r;
				}
				basicshader.use();
				basicshader.setVec3("light.position", glm::vec3(fire.x, fire.y + 1.0f, fire.z));
				basicshader.setVec3("viewPos", camera.Position);
				basicshader.setVec3("light.color", glm::vec3(1.0f,0.0f,0.0f));

				basicshader.setVec3("light.ambient", 0.3f, 0.3f, 0.3f);
				basicshader.setVec3("light.diffuse", 1.0f, 1.0f, 1.0f);
				basicshader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
				basicshader.setFloat("light.constant", 1.0f);
				basicshader.setFloat("light.linear", 0.09f);
				basicshader.setFloat("light.quadratic", 0.032f);
				
		
				basicshader.setFloat("material.shininess", 32.0f);
				
				basicshader.setMat4("projection", projection);
				basicshader.setMat4("view", view);
	
				glm::mat4 bombmodel = glm::mat4(1.0f);
				bombmodel = glm::translate(bombmodel, fire);
				bombmodel = glm::scale(bombmodel, glm::vec3(0.001f, 0.001f, 0.001f));
				bombmodel = glm::rotate(bombmodel, glm::radians(-r*2500-90.0f), glm::vec3(1.0, 0.0, 0.0));
				
				basicshader.setMat4("model", bombmodel);
				glActiveTexture(GL_TEXTURE6);
				glBindTexture(GL_TEXTURE_2D, bombdiffuse);
				glActiveTexture(GL_TEXTURE7);
				glBindTexture(GL_TEXTURE_2D, bombspecular);
				bomb.Draw(basicshader);
				
			}
			
		
				break;
			}
		default:
			break;
		} 

		if (step <= 1) {
			glDepthFunc(GL_LEQUAL);  
			skyboxShader.use();
			view = glm::mat4(glm::mat3(camera.GetViewMatrix())); 
			skyboxShader.setMat4("view", view);
			skyboxShader.setMat4("projection", projection);
			if (step == 1)
				skyboxShader.setVec4("fogcolor", glm::vec4(s, s, s, 0));
		
			glBindVertexArray(skyboxVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS); 
		}
		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}




void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		isOn = true;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		showBomb = true;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		start = true;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrComponents;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

#include <Renderer.h>
#include <CustomShape.h>

Renderer::Renderer()
{
	Shader newShader("Shaders/vertex.shad", "Shaders/fragment.shad");
    Shader newLineShader("Shaders/linevertex.shad", "Shaders/linefragment.shad");
    defaultShader = newShader;
    lineShader = newLineShader;

    float vertices[] = {
        // positions         // texture coords
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f  // top left 
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Renderer::RenderObject(Object& object, const glm::mat4& view, const glm::mat4& projection, std::vector<Light>& lights, std::vector<ShadowCaster>& casters)
{
    if (object.color.w == 0)
    {
        return;
    }

	defaultShader.use();
	defaultShader.setTexture2D("texture1", object.texture, 0);

	glm::mat4 transform = glm::mat4(1.0f);

	transform = glm::translate(transform, glm::vec3(object.position.x, object.position.y, object.position.z));
	transform = glm::rotate(transform, object.rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::scale(transform, glm::vec3(object.scale.x, object.scale.y, 1.0f));

	defaultShader.setMat4("transform", transform);
	defaultShader.setMat4("view", view);
	defaultShader.setMat4("projection", projection);

	defaultShader.setVec4("ourColor", object.color);
	defaultShader.setVec4("globalLightColor", glm::vec4(1.0f));

    defaultShader.setInt("numLights", lights.size());
    for (int i = 0; i < lights.size(); ++i)
    {
        std::string baseName = "lights[" + std::to_string(i) + "]";
        defaultShader.setVec3(baseName + ".position", lights[i].position);
        defaultShader.setFloat(baseName + ".innerRadius", lights[i].innerRadius);
        defaultShader.setFloat(baseName + ".outerRadius", lights[i].outerRadius);
        defaultShader.setVec4(baseName + ".color", lights[i].color);
        defaultShader.setFloat(baseName + ".intensity", lights[i].intensity);
    }

    defaultShader.setInt("numShadowCasters", casters.size());

    for (unsigned int i = 0; i < casters.size(); i++)
    {
        glm::vec2 shadowPos = casters[i].position;
        glm::vec2 size = casters[i].scale;
        float bodyAngle = casters[i].rotation;
        defaultShader.setVec3("shadowCasters[" + std::to_string(i) + "].position", glm::vec3(shadowPos.x, shadowPos.y, 0.0f));
        defaultShader.setVec2("shadowCasters[" + std::to_string(i) + "].size", size);
        defaultShader.setFloat("shadowCasters[" + std::to_string(i) + "].angle", bodyAngle);
        defaultShader.setBool("shadowCasters[" + std::to_string(i) + "].isFlipped", casters[i].isFlipped);
        defaultShader.setTexture2D("shadowCasterTextures[" + std::to_string(i) + "]", casters[i].texture, i + 1);
    }

    float vertices[] = {
        // positions     // texture coords
        0.5f,  0.5f, 0.0f, object.isFlipped ? 0.0f : 1.0f, 1.0f,   // top right (flipped if true)
        0.5f, -0.5f, 0.0f, object.isFlipped ? 0.0f : 1.0f, 0.0f,   // bottom right (flipped if true)
       -0.5f, -0.5f, 0.0f, object.isFlipped ? 1.0f : 0.0f, 0.0f,   // bottom left (flipped if true)
       -0.5f,  0.5f, 0.0f, object.isFlipped ? 1.0f : 0.0f, 1.0f    // top left (flipped if true)
    };

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::RenderBodyVertices(Shader& shader, Camera& camera, Object& object, const glm::mat4& view, const glm::mat4& projection)
{
    std::vector<glm::vec2> vertices;

    // Iterate over all fixtures in the object's body
    for (b2Fixture* f = object.body->GetFixtureList(); f; f = f->GetNext()) {
        // Check if the fixture is of polygon shape
        if (f->GetType() == b2Shape::e_polygon) {
            b2PolygonShape* shape = (b2PolygonShape*)f->GetShape();
            // Get world coordinates for each vertex in the shape
            for (int i = 0; i < shape->m_count; ++i) {
                b2Vec2 vertex = object.body->GetWorldPoint(shape->m_vertices[i]);
                vertices.push_back(glm::vec2(vertex.x, vertex.y));
            }
        }
    }

    // Create and bind VAO and VBO
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), vertices.data(), GL_STATIC_DRAW);

    // Define vertex attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    // Use the shader program
    shader.use();

    // Set shader uniforms
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setVec4("lineColor", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);

    // Draw the polygon lines
    if (!vertices.empty()) {
        glDrawArrays(GL_LINE_LOOP, 0, vertices.size());

        // Draw points to highlight the vertices
        glPointSize(10.0f);
        glDrawArrays(GL_POINTS, 0, vertices.size());
    }

    // Unbind and delete VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void Renderer::RenderCustomShape(Shader& shader, Camera& camera, Object& object, const glm::mat4& view, const glm::mat4& projection)
{    
    std::vector<glm::vec2> vertices;

    // Iterate over all fixtures in the object's body
    for (b2Fixture* f = object.body->GetFixtureList(); f; f = f->GetNext()) {
        if (f->GetType() == b2Shape::e_polygon) {
            b2PolygonShape* shape = (b2PolygonShape*)f->GetShape();
            for (int i = 0; i < shape->m_count; ++i) {
                b2Vec2 vertex = object.body->GetWorldPoint(shape->m_vertices[i]);
                vertices.push_back(glm::vec2(vertex.x, vertex.y));
            }
        }
    }
    
    // Create and bind VAO and VBO
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), vertices.data(), GL_STATIC_DRAW);

    // Define vertex attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    // Use the shader program
    shader.use();

    // Set shader uniforms
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setVec4("lineColor", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);

    // Draw the polygon outline
    glLineWidth(2.0f);
    glDrawArrays(GL_LINE_LOOP, 0, vertices.size());

    // Draw points to highlight the vertices
    glPointSize(10.0f);  
    glDrawArrays(GL_POINTS, 0, vertices.size());

    // Unbind and delete VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void Renderer::RenderRope(std::vector<glm::vec2> points, const glm::mat4& view, const glm::mat4& projection)
{
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec2), points.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    lineShader.use();

    lineShader.setMat4("view", view);
    lineShader.setMat4("projection", projection);
    lineShader.setVec4("lineColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    glm::mat4 model = glm::mat4(1.0f);
    lineShader.setMat4("model", model);

    // Draw lines
    glDrawArrays(GL_LINE_LOOP, 0, points.size());

    // Draw points
    glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, points.size());

    // Cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void Renderer::RenderTilemap(Tilemap& tilemap, const glm::mat4& view, const glm::mat4& projection)
{
    defaultShader.use();

    for (Tile tile : tilemap.tiles)
    {
        defaultShader.setTexture2D("texture1", tile.texture, 0);

        glm::mat4 transform = glm::mat4(1.0f);

        transform = glm::translate(transform, glm::vec3(tile.position.x, tile.position.y, 0.0f));
        transform = glm::scale(transform, glm::vec3(tilemap.scale.x, tilemap.scale.y, 1.0f));

        defaultShader.setMat4("transform", transform);
        defaultShader.setMat4("view", view);
        defaultShader.setMat4("projection", projection);

        defaultShader.setVec4("ourColor", glm::vec4(1.0f));
        defaultShader.setVec4("globalLightColor", glm::vec4(1.0f));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}
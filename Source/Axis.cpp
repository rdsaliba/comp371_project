#include "Axis.h"

Axis::Axis()
{}

Axis::Axis(float size, float gridUnit, GLuint * vaoArray, GLuint * vboArray)
{
    _xAxisVertexArray[0] = glm::vec3(0.0f, 0.0f, 0.0f);
    _xAxisVertexArray[1] = glm::vec3(1.0f, 0.0f, 0.0f); // Red
    _xAxisVertexArray[2] = glm::vec3(5 * gridUnit, 0.0f, 0.0f); //3 units in lengths
    _xAxisVertexArray[3] = glm::vec3(1.0f, 0.0f, 0.0f);
    

    _yAxisVertexArray[0] = glm::vec3(0.0f, 0.0f, 0.0f);
    _yAxisVertexArray[1] = glm::vec3(0.0f, 1.0f, 0.0f); //Green
    _yAxisVertexArray[2] = glm::vec3(0.0f, 5 * gridUnit, 0.0f); //3 units in lengths
    _yAxisVertexArray[3] = glm::vec3(0.0f, 1.0f, 1.0f);
    

    _zAxisVertexArray[0] = glm::vec3(0.0f, 0.0f, 0.0f);
    _zAxisVertexArray[1] = glm::vec3(1.0f, 1.0f, 0.0f); //Yellow
    _zAxisVertexArray[2] = glm::vec3(0.0f, 0.0f, 5 * gridUnit); //3 units in lengths
    _zAxisVertexArray[3] = glm::vec3(1.0f, 1.0f, 0.0f);
   
    _vaoArray = vaoArray;
    _vboArray = vboArray;
    };


void Axis::drawAxisLines(int shader, GLuint vao[], float gridUnit, mat4 worldRotationUpdate)
{
    glBindVertexArray(vao[1]);
    mat4 axisMatrix = worldRotationUpdate * mat4(1.0f);

    GLuint worldMatrixLocation = glGetUniformLocation(shader, "worldMatrix");
    //X-axis
    glProgramUniformMatrix4fv(shader, worldMatrixLocation, 1, GL_FALSE, &axisMatrix[0][0]);
    glDrawArrays(GL_LINES, 0, 2);

    //Y-axis
    glBindVertexArray(vao[2]);
    axisMatrix = worldRotationUpdate * glm::mat4(1.0f);
    glProgramUniformMatrix4fv(shader, worldMatrixLocation, 1, GL_FALSE, &axisMatrix[0][0]);
    glDrawArrays(GL_LINES, 0, 2);

    //Z-axis
    glBindVertexArray(vao[3]);
    axisMatrix = worldRotationUpdate * glm::mat4(1.0f);
    glProgramUniformMatrix4fv(shader, worldMatrixLocation, 1, GL_FALSE, &axisMatrix[0][0]);
    glDrawArrays(GL_LINES, 0, 2);
}

void Axis::bindAxis()
{
    //Axis lines
    //X
    glBindVertexArray(_vaoArray[1]);
    glBindBuffer(GL_ARRAY_BUFFER, _vboArray[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_xAxisVertexArray), _xAxisVertexArray, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(1);

    //Y
    glBindVertexArray(_vaoArray[2]);
    glBindBuffer(GL_ARRAY_BUFFER, _vboArray[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_yAxisVertexArray), _yAxisVertexArray, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(1);

    //Z
    glBindVertexArray(_vaoArray[3]);
    glBindBuffer(GL_ARRAY_BUFFER, _vboArray[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_zAxisVertexArray), _zAxisVertexArray, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(1);
}

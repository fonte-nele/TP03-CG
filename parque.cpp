// Parque
/*
 * Feito por:
 *       Felipe Fontenele de Ávila Magalhães 15.1.4331
 *       Matheus Gonçalves Ferreira          15.1.4341
 *       Victor Freitas                      17.2.4254
*/
/** COMANDOS DE CONTROLE
 *       'd': camera em posicao default.
 *       'i': deslocamento para cima.
 *       'k': deslocamento para baixo.
 *       'j': deslocamento lateral para esquerda.
 *       'l': deslocamento lateral para direito.
 *       'w': camera sobe.
 *       's': camera desce.
 *       'a': camera anda para frente (deslocamento horizontal).
 *       'z': camera anda para tras (deslocamento horizontal).
 *       'p': camera situada em posicao elevada 
 *       'c': acender/apagar luz poste das luminarias.
 *       'b': visao de um usuario do brinquedo, para sair dessa visao 'b' novamente.
 *       '1': versao simplificada com outdoor carrossel.
 *       '2': versao simplificada com outdoor skate.
 */
/*
  Link do video para detalhamento do carrossel:
    https://www.youtube.com/watch?v=xG19jKZ9agI
*/
//glm: biblioteca matem�tica para OpenGL
#include <glm/glm.hpp> //vec2, vec3, mat4, etc
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
// arquivo com recursos auxiliares (do livro texto)
using namespace std;
#include "Angel_util.h"
#include "Cilindro.cpp"
#include "Cubo.cpp"
#include "cone.cpp"
#include "Sphere.cpp"
#include "illumination.cpp"
#include "BMPLoader.cpp" //<<<textura

#define M_PI 3.14159265358979323846
#define X 0
#define Y 1
#define Z 2

typedef glm::vec4  color4;
typedef glm::vec4  point4;

GLuint Model, View, Projection, isLightSource, isMoon;
GLuint program;
GLuint textura1, textura2, textura3, textura4, textura5;  // Imagens com a textura carregada!
GLuint withTexture; // Variavel booleana para aplicar textura!

Cubo cubo;
Cilindro cilindro;
Cone cone;
Sphere esfera;
Illumination ilu;

static float angulo=0.0;    // Angulo rotacao do skate
static float movSkate=0.0;  // Controlar a ida e volta do skate
static float altSkate=1.0;  // Controlar a altura do skate
static int topo = 0;        // Controlar o momento certo da rotacao do skate
static bool branco = false; // Cor do poste!
static bool brinq = false;  // Visao do brinquedo!
static bool tecla1 = false;  // Equipamento carrosel!
static bool tecla2 = false;  // Equipamento skate!

glm::vec4 light_position1(  50.0f, 12.0f, 0.0f, 1.0f );
glm::vec4 light_position2( -50.0f, 12.0f, 0.0f, 1.0f );

//----------------------------------------------------------------------------

void desenhaFonteDeLuz(glm::vec4 pos, float r)
{   // desenha uma esfera de raio r, representando a fonte de luz
    glm::mat4 luz;
    luz = glm::translate(luz, glm::vec3(pos.x,pos.y,pos.z));
    luz = glm::scale(luz, glm::vec3(r,r,r));
    glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(luz));
    glUniform1i(isLightSource, true);
    esfera.draw();
    glUniform1i(isLightSource, false);
}

//----------------------------------------------------------------------------

void cavalo(glm::mat4 M) {
    // Modelo com parte inferior do corpo (barriga) situada
    // em y=0.0, ou seja, no ch�o
    // Matriz M: transformacoes para posicionar o cavalo

    //Dimensoes das partes do cavalo
    float xCorpo=1.5, yCorpo=1.0, zCorpo=0.5;
    float xPesc=0.3,  yPesc=0.6,  zPesc=0.2;  //pesco�o
    float xCab=0.5,   yCab=0.2,   zCab=0.2;   //cabe�a
    M = glm::translate(M, glm::vec3(0.0, yCorpo/2.0, 0.0));

   // corpo (orientado segundo o eixo X)
   glm::mat4 matCorpo = M;
   matCorpo = glm::scale(matCorpo, glm::vec3(xCorpo, yCorpo, zCorpo));
   glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matCorpo));
   cubo.desenhar();

   // pescoco
   glm::mat4 matPesc = M;
   matPesc = glm::translate(matPesc, glm::vec3(xCorpo/2-xPesc/2, yCorpo/2+yPesc/2, 0.0));
   matPesc = glm::scale(matPesc, glm::vec3(xPesc, yPesc, zPesc));
   glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matPesc));
   cubo.desenhar();

   // cabeca
   glm::mat4 matCab = M;
   matCab = glm::translate(matCab, glm::vec3(xCorpo/2.0+xCab/2.0-xPesc, yCorpo/2.0+yPesc+yCab/2.0, 0.0));
   matCab = glm::scale(matCab, glm::vec3(xCab, yCab, zCab));
   glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matCab));
   cubo.desenhar();
}

void sustentacao(glm::mat4 posicao)
{
    // plataforma (placa de dimensoes 40x0.8x22, com face inferior em y=0.0)
    float altPlat=0.8;
    glm::mat4 matPlat(posicao);
    glUniform1i(withTexture, true);
    glBindTexture(GL_TEXTURE_2D, textura1);
    matPlat = glm::translate(matPlat, glm::vec3(0.0, altPlat/2.0, 0.0));
    matPlat = glm::scale(matPlat, glm::vec3(40.0, 0.8, 28.0));
    glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matPlat));
    cubo.desenhar();
    glUniform1i(withTexture, false);

    // mastros sustentacao skate
    float altMastro = 15.0;
    float diamMastro = 0.3;  //diametro do mastro
    float posX = -20.0, posZ = -4.0;

    glm::mat4 matMastro, rotMastro(posicao);
    for (int i = 0; i < 4; i++){
       matMastro = glm::translate(rotMastro, glm::vec3(posX,altPlat/2.0,posZ));
       matMastro = glm::rotate(matMastro, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
       matMastro = glm::scale(matMastro, glm::vec3(diamMastro,diamMastro,altMastro));
       glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matMastro));
       ilu.matDiffuse(0.0, 0.0, 1.0, 1.0);
       cilindro.desenhar();
       ilu.stdMaterial();

        if(i == 0)
          posZ = 4.0;
        else if(i == 1)
          posX = 20.0, posZ = -4.0;
        else if(i == 2)
          posX = 20.0, posZ = 4.0;
    }
    int i = 1;
    posX = -19.5;
    float aux;
    while(posX < 0.0){
        matMastro = glm::translate(rotMastro, glm::vec3(posX,altPlat/2.0,-4.0));
        matMastro = glm::rotate(matMastro, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
        matMastro = glm::scale(matMastro, glm::vec3(diamMastro-0.1,diamMastro-0.1,altMastro-0.5*i));
        glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matMastro));
        ilu.matDiffuse(1.0, 1.0, 0.0, 1.0);
        cilindro.desenhar();
        ilu.stdMaterial();

        matMastro = glm::translate(rotMastro, glm::vec3(posX,altPlat/2.0,4.0));
        matMastro = glm::rotate(matMastro, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
        matMastro = glm::scale(matMastro, glm::vec3(diamMastro-0.1,diamMastro-0.1,altMastro-0.5*i));
        glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matMastro));
        ilu.matDiffuse(1.0, 1.0, 0.0, 1.0);
        cilindro.desenhar();
        ilu.stdMaterial();
        

        matMastro = glm::translate(rotMastro, glm::vec3(posX-0.375,altPlat/2.0,-4.0));
        matMastro = glm::rotate(matMastro, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
        matMastro = glm::scale(matMastro, glm::vec3(diamMastro-0.1,diamMastro-0.1,altMastro-0.5*i+0.25));
        glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matMastro));
        ilu.matDiffuse(1.0, 1.0, 0.0, 1.0);
        cilindro.desenhar();
        ilu.stdMaterial();

        matMastro = glm::translate(rotMastro, glm::vec3(posX-0.375,altPlat/2.0,4.0));
        matMastro = glm::rotate(matMastro, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
        matMastro = glm::scale(matMastro, glm::vec3(diamMastro-0.1,diamMastro-0.1,altMastro-0.5*i+0.25));
        glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matMastro));
        ilu.matDiffuse(1.0, 1.0, 0.0, 1.0);
        cilindro.desenhar();
        ilu.stdMaterial();
        posX += 0.75;
        i++;
    }
    i = 1;
    posX = 19.5;
    while(posX > 0.0){
        matMastro = glm::translate(rotMastro, glm::vec3(posX,altPlat/2.0,-4.0));
        matMastro = glm::rotate(matMastro, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
        matMastro = glm::scale(matMastro, glm::vec3(diamMastro-0.1,diamMastro-0.1,altMastro-0.5*i));
        glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matMastro));
        ilu.matDiffuse(1.0, 1.0, 0.0, 1.0);
        cilindro.desenhar();
        ilu.stdMaterial();

        matMastro = glm::translate(rotMastro, glm::vec3(posX,altPlat/2.0,4.0));
        matMastro = glm::rotate(matMastro, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
        matMastro = glm::scale(matMastro, glm::vec3(diamMastro-0.1,diamMastro-0.1,altMastro-0.5*i));
        glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matMastro));
        ilu.matDiffuse(1.0, 1.0, 0.0, 1.0);
        cilindro.desenhar();
        ilu.stdMaterial();

        matMastro = glm::translate(rotMastro, glm::vec3(posX-0.375,altPlat/2.0,-4.0));
        matMastro = glm::rotate(matMastro, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
        matMastro = glm::scale(matMastro, glm::vec3(diamMastro-0.1,diamMastro-0.1,altMastro-0.5*i-0.25));
        glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matMastro));
        ilu.matDiffuse(1.0, 1.0, 0.0, 1.0);
        cilindro.desenhar();
        ilu.stdMaterial();

        matMastro = glm::translate(rotMastro, glm::vec3(posX-0.375,altPlat/2.0,4.0));
        matMastro = glm::rotate(matMastro, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
        matMastro = glm::scale(matMastro, glm::vec3(diamMastro-0.1,diamMastro-0.1,altMastro-0.5*i-0.25));
        glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matMastro));
        ilu.matDiffuse(1.0, 1.0, 0.0, 1.0);
        cilindro.desenhar();
        ilu.stdMaterial();
        posX -= 0.75;
        i++;
    }

    float aux1 = 8.0, aux2 = 7.0;
    glm::mat4 matDegrau, rotDegrau(posicao);
    for (int i = 0; i < 3; i++){
       matDegrau = glm::translate(rotDegrau, glm::vec3(0.0, altPlat/2.0 + (i+1)*0.8, aux1));
       matDegrau = glm::scale(matDegrau, glm::vec3(15.0, 0.8, aux2));
       glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matDegrau));
       ilu.matDiffuse(1.0, 0.0, 0.0, 1.0);
       cubo.desenhar();
       ilu.stdMaterial();

       matDegrau = glm::translate(rotDegrau, glm::vec3(0.0, altPlat/2.0 + (i+1)*0.8, -aux1));
       matDegrau = glm::scale(matDegrau, glm::vec3(15.0, 0.8, -aux2));
       glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matDegrau));
       ilu.matDiffuse(1.0, 0.0, 0.0, 1.0);
       cubo.desenhar();
       ilu.stdMaterial();

       aux1 -= 1.0;
       aux2 -= ((i+1)*0.8);
    }
}

void movimentoSkate(void)
{
    bool flag[2] = {true, false};
    static bool aux1 = false, aux2 = false;
   
    static float deslocamento = 0.015;
    static float deslocamento2 = 0.020;
    static int contador = 0, cont = 0, cont1 = 0;

    if(!aux1 && !aux2){
        if(altSkate >= 4.0 || altSkate <= 0.9){
            deslocamento *= -1;
            if(flag[contador]){
                deslocamento2 *= -1;
            }
            cont++;
            contador++;
            if(contador == 2){
                contador = 0;
            }
            if(cont == 4){
                aux1 = true;
                cont = 0;
                cont1 = 1;
            }
        }
    }
    if(!aux2 && aux1){
        if((altSkate >= 6.5 || altSkate <= 0.9) && cont1 != 1){
            deslocamento *= -1;
            if(flag[contador]){
                deslocamento2 *= -1;
            }
            cont++;
            contador++;
            if(contador == 2){
                contador = 0;
            }
            if(cont == 4){
                aux2 = true;
                cont = 0;
                cont1 = 1;
            }
        }
        else
            cont1 = 0;
    }
    if(aux1 && aux2){
        if((altSkate >= 11.0 || altSkate <= 0.9) && cont1 != 1){
            deslocamento *= -1;
            if(flag[contador])
              deslocamento2 *= -1;
            
            contador++;
            if(contador == 2)
              contador = 0;
            topo = 1;
        }
        else
            cont1 = 0;
    }

    altSkate += deslocamento;
    movSkate += deslocamento2;
        
    angulo += 0.2;
    if(angulo > 360)
        angulo = 0.0;
}

void skate(glm::mat4 posicao)
{
    glm::mat4 matSkate(posicao);
    matSkate = glm::translate(matSkate, glm::vec3(0.0, 4.0, 0.0));
    matSkate = glm::translate(matSkate, glm::vec3(movSkate,altSkate,0.0));
    if(topo){
        matSkate = glm::rotate(matSkate, glm::radians(angulo), glm::vec3(0.0f,1.0f,0.0f));
    }
    matSkate = glm::scale(matSkate, glm::vec3(10.0, 0.25, 7.9));
    glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matSkate));
    cubo.desenhar();

    float posX = -2.0, posZ = -1.5;
    glm::mat4 matCadeira, rotCadeira(posicao), inversa;
    for (int i = 0; i < 4; i++){
       matCadeira = glm::translate(rotCadeira, glm::vec3(posX,4.25,posZ));
       matCadeira = glm::translate(matCadeira, glm::vec3(movSkate,altSkate,0.0));
       if(topo){
          matCadeira = glm::rotate(matCadeira, glm::radians(angulo), glm::vec3(0.0f,1.0f,0.0f));
       }
       matCadeira = glm::scale(matCadeira, glm::vec3(2.5,0.5,1.5));
       if(brinq){
          inversa = glm::inverse(matCadeira);
          glUniformMatrix4fv(View,1,GL_FALSE, glm::value_ptr(inversa));
       }
       else{
          glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matCadeira));
       }
       cubo.desenhar();

        if(i == 0)
          posZ = 1.5;
        else if(i == 1)
          posX = 2.0, posZ = -1.5;
        else if(i == 2)
          posX = 2.0, posZ = 1.5;
    }
    movimentoSkate();
}

void carrossel(glm::mat4 posicao)
{
    static float angCar=0.0; // rotacao do carrossel
    float altMedia=4.0;      // altura media dos cavalos
    float amplitude=2.0;     // amplitude da variacao da altura
    float raioCar=9.0;       // raio da circunferencia do carrossel

    angCar += 0.001;
    if( angCar > 2.0*M_PI ) angCar -= 2.0*M_PI;

    // plataforma (placa de dimensoes 21x0.8x21, com face inferior em y=0.0)
    float altPlat=0.8;
    glm::mat4 matPlat(posicao);
    glUniform1i(withTexture, true);
    glBindTexture(GL_TEXTURE_2D, textura1);
    matPlat = glm::translate(matPlat, glm::vec3(0.0, altPlat/2.0, 0.0));
    matPlat = glm::scale(matPlat, glm::vec3(21.0, 0.8, 21.0));
    glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matPlat));
    cubo.desenhar();
    glUniform1i(withTexture, false);

    // mastro (eixo) do carrossel
    float altMastro = 10.0;
    float diamMastro = 0.3;  //diametro do mastro
    glm::mat4 matMastro(posicao);
    matMastro = glm::rotate(matMastro, angCar, glm::vec3(0.0f,1.0f,0.0f));
    matMastro = glm::rotate(matMastro, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
    matMastro = glm::scale(matMastro, glm::vec3(diamMastro,diamMastro,altMastro));
    glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matMastro));
    cilindro.desenhar();

    // cobertura
    glm::mat4 matCob(posicao);
    matCob = glm::translate(matCob, glm::vec3(0.0, 10.0, 0.0));
    matCob = glm::rotate(matCob, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
    matCob = glm::scale(matCob, glm::vec3(10.0, 10.0, 2.0));
    glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matCob));
    cone.desenhar();

    // 8 cavalos com hastes
    float altCav = altMedia + amplitude*sin(2.0*angCar);
    float angDefasagem = 2.0*M_PI/8.0; // defasagem entre os cavalos
    float diamHaste = 0.1;
    glm::mat4 matCavalo, rotCavalo(posicao), matHaste;
    rotCavalo = glm::rotate(rotCavalo, angCar, glm::vec3(0.0f,1.0f,0.0f));
    for (int i = 0; i < 8; i++){
       rotCavalo = glm::rotate(rotCavalo, angDefasagem, glm::vec3(0.0f,1.0f,0.0f));
       matCavalo = glm::translate(rotCavalo, glm::vec3(0.0f,altCav,raioCar));
       cavalo(matCavalo);

       matHaste = glm::translate(rotCavalo, glm::vec3(0.0f,0.0f,raioCar));
       matHaste = glm::rotate(matHaste, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
       matHaste = glm::scale(matHaste, glm::vec3(diamHaste,diamHaste,altMastro));
       glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matHaste));
       cilindro.desenhar();
    }
}

void chao()
{
    // chao (placa de dimensoes 80x0.3x80, com face superior em y=0.0)
    glm::mat4 matChao;
    glUniform1i(withTexture, true);
    glBindTexture(GL_TEXTURE_2D, textura2);
    matChao = glm::translate(matChao, glm::vec3(0.0, -0.15, 0.0));
    matChao = glm::scale(matChao, glm::vec3(80.0, 0.3, 80.0));
    glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matChao));
    cubo.desenhar();
    glUniform1i(withTexture, false);
}

void lua(glm::vec4 pos, float r)
{   // desenha a lua, representando uma fonte de luz
    glm::mat4 lua1;
    lua1 = glm::translate(lua1, glm::vec3(pos.x,pos.y,pos.z));
    lua1 = glm::scale(lua1, glm::vec3(r,r,r));
    glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(lua1));
    glUniform1i(isLightSource, true);
    glUniform1i(isMoon, true);
    esfera.draw();
    glUniform1i(isLightSource, false);
    glUniform1i(isMoon, false);
}

void luminarias(glm::mat4 posicao)
{
    float altLum = 25.0;  // Altura luminaria
    float diamLum = 0.5;  // Diametro da luminaria
    glm::mat4 matLum(posicao);
    matLum = glm::rotate(matLum, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
    matLum = glm::scale(matLum, glm::vec3(diamLum,diamLum,altLum));
    glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matLum));
    ilu.matDiffuse(0.5, 0.5, 0.5, 1.0);
    cilindro.desenhar();
    ilu.stdMaterial();

    glm::mat4 matGlobo(posicao);
    matGlobo = glm::translate(matGlobo, glm::vec3(0.0,25.0,0.0));
    matGlobo = glm::rotate(matGlobo, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
    matGlobo = glm::scale(matGlobo, glm::vec3(1.5,1.5,1.5));
    glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matGlobo));
    if(!branco){  // Iluminacao do poste!
      glUniform1i(isLightSource, true);
    }
    else{  // Poste com luz apagadas!
      ilu.matDiffuse(1.0, 1.0, 1.0, 1.0);
    }
    esfera.draw();
    if(!branco){
      glUniform1i(isLightSource, false);
    }
    else{
      ilu.stdMaterial();
    }
}

void outdoor(glm::mat4 posicao)
{
    float altOut = 24.0;  // Altura outdoor
    float diamOut = 0.35;  // Diametro da outdoor
    glm::mat4 matOut(posicao);
    matOut = glm::rotate(matOut, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
    matOut = glm::scale(matOut, glm::vec3(diamOut,diamOut,altOut));
    glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matOut));
    cilindro.desenhar();

    glm::mat4 matOut2(posicao);
    matOut2 = glm::translate(matOut2, glm::vec3(-21.0,0.0,0.0));
    matOut2 = glm::rotate(matOut2, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
    matOut2 = glm::scale(matOut2, glm::vec3(diamOut,diamOut,altOut));
    glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matOut2));
    cilindro.desenhar();

    glm::mat4 matPlaca;
    glUniform1i(withTexture, true);
    if(!tecla1 && !tecla2)
        glBindTexture(GL_TEXTURE_2D, textura3);
    else if(tecla1)
        glBindTexture(GL_TEXTURE_2D, textura4);
    else if(tecla2)
        glBindTexture(GL_TEXTURE_2D, textura5);
    matPlaca = glm::translate(matPlaca, glm::vec3(-26.5,18.0,-35.0));
    matPlaca = glm::rotate(matPlaca, glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
    matPlaca = glm::scale(matPlaca, glm::vec3(21.0,0.5,12.0));
    glUniformMatrix4fv(Model,1,GL_FALSE, glm::value_ptr(matPlaca));
    cubo.desenhar();
    glUniform1i(withTexture, false);
}

void exibe( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    int pos[] = {1,2};  // Vetor de posicao!
    float angulo[] = {-30.0, 10.0}; // Angulo de orientacao!
    
    chao();
    
    if(!tecla1 && !tecla2){
        glm::mat4 posicaoCarrosel;
        posicaoCarrosel = glm::translate(posicaoCarrosel, glm::vec3(0.0,0.0,angulo[0]));
        carrossel(posicaoCarrosel);

        glm::mat4 posicaoSkate;
        posicaoSkate = glm::translate(posicaoSkate, glm::vec3(0.0,0.0,angulo[1])); // -30, 0, 0
        sustentacao(posicaoSkate);
        skate(posicaoSkate);

        glm::vec4 pos(20.0, 45.0, -70.0, 1.0);
        lua(pos, 3.5);

        glm::mat4 posLum;
        posLum = glm::translate(posLum, glm::vec3(30.0,0.0,-35.0));
        luminarias(posLum);
        posLum = glm::translate(posLum, glm::vec3(-60.0,0.0,70.0));
        luminarias(posLum);

        glm::mat4 posOutdoor;
        posOutdoor = glm::translate(posOutdoor, glm::vec3(-16.0,0.0,-35.0));
        outdoor(posOutdoor);
    }
    else{
        glm::mat4 posOutdoor;
        posOutdoor = glm::translate(posOutdoor, glm::vec3(-16.0,0.0,-35.0));
        outdoor(posOutdoor);
    }

    desenhaFonteDeLuz(light_position1, 0.5);
    desenhaFonteDeLuz(light_position2, 0.5);

    glFlush();
    glutSwapBuffers();
}

// ============================================= Camera ======================================================
bool troca_camera;
static GLfloat pos[] = {0.0,10.0,50.0};
static GLfloat alfa = 0.0;  // angulo de rotação da câmera em torno de Y
                               // alfa=0.0 ==> câmera aponta para -z
static GLfloat gama = 0.0;  // angulo de inclinacao vertical da direcao de observacao

void camera(bool troca){
    alfa = 0.0;
    
    if(troca){
        pos[X] = 0.0, pos[Y] = 40.0, pos[Z] = 50.0;
        gama = -0.45;
    }
    else{
        pos[X] = 0.0, pos[Y] = 10.0, pos[Z] = 50.0;
        gama = 0.0;
    }
}

void posicionaCamera(unsigned char tecla) 
{   
   static GLfloat dir[] = {0.0,0.0,-1.0}; // deslocamento horizontal!
   GLfloat passo = 0.1;
   
   switch( tecla ) {
     case 'd': // camera em posicao default
               pos[X] = 0.0, pos[Y] = 10.0, pos[Z] = 50.0;
               break;
     case 'i': gama += 0.02;      // camera inclina para cima
               break;
     case 'k': gama -= 0.02;      // camera inclina para baixo
               break;
     case 'j': alfa += 0.02;      // camera gira para esquerda
               break;
     case 'l': alfa -= 0.02;      // camera gira para direita
               break;
     case 'a': pos[X] += passo*dir[X]; // camera anda para a frente
               pos[Z] += passo*dir[Z];
               break;
     case 'z': pos[X] -= passo*dir[X]; // camera anda para trás
               pos[Z] -= passo*dir[Z];
               break;
     case 'w': pos[Y] += passo*0.2;     // camera sobe
               break;
     case 's': pos[Y] -= passo*0.2;     // camera desce
               break;
     case 'p': troca_camera = !troca_camera; // troca posicao da camera
               camera(troca_camera);
               break;
   }
   if (tecla=='j' || tecla=='l') {  // vetor de direção mantido com módulo=1.0
     dir[X] = cos(alfa+M_PI); // alfa = 0.0 coincide com -Z
     dir[Z] = sin(alfa-M_PI);
   }
   glm::mat4 matVis;
   matVis = glm::rotate(matVis, -gama, glm::vec3(1.0f,0.0f,0.0f));
   matVis = glm::rotate(matVis, -alfa, glm::vec3(0.0f,1.0f,0.0f));
   matVis = glm::translate(matVis, glm::vec3(-pos[X], -pos[Y], -pos[Z]));
   glUniformMatrix4fv(View,1,GL_FALSE, glm::value_ptr(matVis));
   glutPostRedisplay();
}

void teclado( unsigned char tecla, int x, int y )
{
    if (tecla==033)   // Escape Key
      exit( EXIT_SUCCESS );
    else
    {
        if(tecla == 'c')  // Acender/apagar luz das luminarias! 
            if(!branco)
              branco = true;
            else
              branco = false;
        if(tecla == 'b')  // Entrar/sair da visao de um usuario do brinquedo! 
            if(!brinq)
              brinq = true;
            else
              brinq = false;
        if(tecla == '1')  // Equipamento carrossel! 
            if(!tecla1)
              tecla1 = true;
            else
              tecla1 = false;
        if(tecla == '2')  // Equipamento skate!
            if(!tecla2)
              tecla2 = true;
            else
              tecla2 = false;
        posicionaCamera(tecla);
    }
}

//----------------------------------------------------------------------------

void idle( void )
{
   exibe();
}

//----------------------------------------------------------------------------

// OpenGL initialization
void init()
{
    // Load shaders and use the resulting shader program
    program = InitShader( "genericoVshaderTextura.glsl",
                          "genericoFshaderTextura.glsl" );
    glUseProgram( program );

    cubo.criaCubo(program, "vPosition", "vNormal", "vTexCoords");
    cilindro.criaCilindro(program, "vPosition", "vNormal");
    cone.criaCone(program, "vPosition", "vNormal");
    esfera.init(program, "vPosition", "vNormal");


    ilu = Illumination(program);
    ilu.lightPositions(light_position1, light_position2);
//**************************************************
    // Initialize shader lighting parameters
    glm::vec4 light_ambient( 0.2f, 0.2f, 0.2f, 1.0f );
    glm::vec4 light_diffuse( 1.0f, 1.0f, 1.0f, 1.0f );
    glm::vec4 light_specular( 1.0f, 1.0f, 1.0f, 1.0f );

    glm::vec4 material_ambient( 1.0f, 0.0f, 1.0f, 1.0f );
    glm::vec4 material_diffuse( 0.8f, 0.6f, 0.0f, 1.0f );
    glm::vec4 material_specular( 0.9f, 0.7f, 0.0f, 1.0f );
    float  material_shininess = 100.0f;

    color4 ambient_product = light_ambient * material_ambient;
    color4 diffuse_product = light_diffuse * material_diffuse;
    color4 specular_product = light_specular * material_specular;

    glUniform4fv( glGetUniformLocation(program, "AmbientProduct"),
      1, glm::value_ptr(ambient_product ));
    glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"),
      1, glm::value_ptr(diffuse_product ));
    glUniform4fv( glGetUniformLocation(program, "SpecularProduct"),
      1, glm::value_ptr(specular_product ));

    glUniform4fv( glGetUniformLocation(program, "LightPosition1"),
      1, glm::value_ptr(light_position1 ));
    glUniform4fv( glGetUniformLocation(program, "LightPosition2"),
      1, glm::value_ptr(light_position2 ));

    glUniform1f( glGetUniformLocation(program, "Shininess"),
     material_shininess );

    // Criar objetos de textura
    withTexture = glGetUniformLocation(program, "withTexture"); // Variável Booleana para aplicar textura!
    BMPClass bmp, bmp1; //<<<textura
    BMPLoad("ladrilho.bmp",bmp); //<<<textura

    glGenTextures(1, &textura1); //<<<textura
    glBindTexture(GL_TEXTURE_2D, textura1); //<<<textura
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,bmp.width,bmp.height,0,GL_RGB,GL_UNSIGNED_BYTE,bmp.bytes);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  //ou GL_LINEAR  //<<<textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  //ou GL_LINEAR  //<<<textura

    BMPLoad("grama.bmp",bmp1); //<<<textura

    glGenTextures(1, &textura2); //<<<textura
    glBindTexture(GL_TEXTURE_2D, textura2); //<<<textura
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,bmp1.width,bmp1.height,0,GL_RGB,GL_UNSIGNED_BYTE,bmp1.bytes);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  //ou GL_LINEAR  //<<<textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  //ou GL_LINEAR  //<<<textura

    BMPLoad("thumbnail.bmp",bmp); //<<<textura

    glGenTextures(1, &textura3); //<<<textura
    glBindTexture(GL_TEXTURE_2D, textura3); //<<<textura
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,bmp.width,bmp.height,0,GL_RGB,GL_UNSIGNED_BYTE,bmp.bytes);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  //ou GL_LINEAR  //<<<textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  //ou GL_LINEAR  //<<<textura

    BMPLoad("carrossel.bmp",bmp); //<<<textura

    glGenTextures(1, &textura4); //<<<textura
    glBindTexture(GL_TEXTURE_2D, textura4); //<<<textura
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,bmp.width,bmp.height,0,GL_RGB,GL_UNSIGNED_BYTE,bmp.bytes);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  //ou GL_LINEAR  //<<<textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  //ou GL_LINEAR  //<<<textura

    BMPLoad("skate.bmp",bmp); //<<<textura

    glGenTextures(1, &textura5); //<<<textura
    glBindTexture(GL_TEXTURE_2D, textura5); //<<<textura
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,bmp.width,bmp.height,0,GL_RGB,GL_UNSIGNED_BYTE,bmp.bytes);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  //ou GL_LINEAR  //<<<textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);  //ou GL_LINEAR  //<<<textura

    GLuint tex_loc; //<<<textura
    tex_loc = glGetUniformLocation(program, "texMap"); //<<<textura
    glUniform1i(tex_loc, 0);

    // Retrieve transformation uniform variable locations
    Model = glGetUniformLocation( program, "Model" );
    Projection = glGetUniformLocation( program, "Projection" );
    View = glGetUniformLocation( program, "View" );

    isLightSource = glGetUniformLocation( program, "isLightSource" );
    isMoon = glGetUniformLocation( program, "isMoon" );

//**************************************************

    glEnable( GL_DEPTH_TEST );
    glClearColor( 0.0, 0.0, 0.0, 1.0 );
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glm::mat4 proj = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 200.0f);
    glUniformMatrix4fv(Projection,1,GL_FALSE, glm::value_ptr(proj));
    glUniform1i(isLightSource, false);

    posicionaCamera('d'); //posicao default
} //init

//----------------------------------------------------------------------------

int main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
    glutCreateWindow( "Parque" );
    glewInit();

    init();

    glutDisplayFunc( exibe );
    glutKeyboardFunc( teclado );
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
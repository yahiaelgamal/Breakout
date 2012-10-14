#include <GLUT/glut.h>
#include <iostream>
#include <stdio.h>
#include <math.h>

#define PI 3.14159265
#define WIDTH 10
#define HEIGHT 8

// ################################### HEADERS #################################
void display(void);
void anim();
void myKeyboard(unsigned char,int,int);
void myMouse(int,int);
void initBricks();
void drawBricks();
bool gameOver();
bool bricksLeft();
// ################################### HEADERS END #############################

// ################################### STRUCTS #################################
struct Ball{
    
    double x = 0;
    double y = 250;
    double side = 10;
    double angle = 90;
    bool used = true;
    
    void applySpeed(){
        if (used){
            y = y + sin(angle * PI / 180);
            x = x + cos(angle * PI / 180);
        }
    }
    void draw(){
        used = y > 0;
        if (used){
            glRectd(x-side/2, y-side/2, x+side/2, y+side/2);
        }
    }
    void info(){
        printf("x: %.1f, y: %.1f ø:%.1f\n", x,y,angle);
    }
    void handleBoundaries(){
        
        if (x >= 300 || x <= -300){
            angle =  180 - angle;
        }
        if (y >= 500){
            angle *=-1;
        }
    }
};

struct Pad{
    double center = 0;
    double y = 5;
    double width = 100;
    double height = 10;
    void draw(){
        
        glColor3f(0.5f, 0.5f, 0.5f);
        glRectd(center-width/2, y, center+width/2, y+height);
        
    }
    bool isHit(Ball b){
        return  b.x <= center+width/2 && b.x >= center-width/2 &&
        b.y >= y && b.y <= y+height;
    }
};

struct Brick{
    double x1;
    double x2;
    double y1;
    double y2;
    bool hit = false;
    float r=rand()+0.25,g=rand()+0.25,b=rand()+0.25;
    
    void draw(){
        glColor3f(r, g, b);
        glRectd(x1, y1, x2, y2);
    }
    bool isHit(Ball b){
        int deltax[] = {1, 1, -1, -1};
        int deltay[] = {1, -1, 1, 1};
        
        for (int i = 0; i < 4; i++){
            if (b.x + deltax[i] * b.side/2 <= x2
                && b.x + deltax[i] * b.side/2 >=x1
                && b.y + deltay[i] * b.side/2 <= y1
                && b.y + deltay[i] * b.side/2 >= y2)
            {
                return true;
            }
        }
        return false;
    }
    void info(){
        printf("x1: %.1f, y1: %.1f, x2: %.1f, y2: %.1f\n", x1, y1, x2, y2);
    }
};
// ################################### STRUCTS END ############################


Brick bricks[HEIGHT][WIDTH];
Ball ball1;
Ball ball2;
Pad pad1;
Pad pad2;
int score=0;
float secs=0;
bool over=false;

bool gameOver(){
    if (over)
        return true;
    if ((!ball1.used && !ball2.used )){
        printf("score is %d, Time is %f FINAL:%f\n", score, secs, score/secs);
        glClearColor(1.0f,0.0f,0.0f,0.0f);
        over = true;
    }else if (!bricksLeft()){
        printf("score is %d, Time is %f FINAL:%f\n", score, secs, score/secs);
        glClearColor(0.0f,1.0f,0.0f,0.0f);
        over=true;
    }
    return over;
}

bool bricksLeft(){
    
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++){
            if (!bricks[i][j].hit)
                return true;
        }
    }
    return false;
}

void printHits(){
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++){
            char c = bricks[i][j].hit? ' ': '.';
            std::cout << c;
        }
        std::cout << std::endl;
    }
    std::cout << "---------------" << std::endl;
}
void initBricks(){
    double startx = -297.5;
    double starty = 497.5;
    for(int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH;j++)
        {
            bricks[i][j].x1 = startx + j * 60;
            bricks[i][j].y1 = starty - i * 20;
            bricks[i][j].x2 = startx + j * 60 + 55;
            bricks[i][j].y2 = starty - (i * 20 + 17);
            
            
            bricks[i][j].r = (double)rand()/0xF0000000 + 0.25;
            bricks[i][j].g = (double)rand()/0xF0000000 + 0.25;
            bricks[i][j].b = (double)rand()/0xF0000000 + 0.25;
        }
    }
    
    bricks[7][5].hit = false;
}

int main(int argc,char** argr){
	glutInit(&argc,argr);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
	glutInitWindowSize(600,500);
	glutInitWindowPosition(50,50);
	glutCreateWindow("Break out");
	glutDisplayFunc(display);
	glutKeyboardFunc(myKeyboard);
    //	glutMouseFunc(myMouse);
    glutPassiveMotionFunc(myMouse);
	glutIdleFunc(anim);
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	gluOrtho2D(-300,300.0,0.0,500.0);
    initBricks();
    
    pad2.y = 25;
	glutMainLoop();
    
    return 0;
}

void display(){
    if (!gameOver()){
        glClear(GL_COLOR_BUFFER_BIT);
        glPushMatrix();
        ball1.draw();
        ball2.draw();
        pad1.draw();
        pad2.draw();
        drawBricks();
        
        ball1.handleBoundaries();
        ball2.handleBoundaries();
        
        glPopMatrix();
        glFlush();
    }else
        glFlush();
}

void drawBricks(){
    int hit = 0;
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++){
            if (!bricks[i][j].hit){
                if (bricks[i][j].isHit(ball1)){
                    bricks[i][j].hit = true;
                    hit = 1;
                    score++;
                    //                    printf("Score is %d\n", score);
                }else if (bricks[i][j].isHit(ball2)){
                    bricks[i][j].hit = true;
                    hit = 2;
                    score++;
                    //                    printf("Score is %d\n", score);
                }
                else {
                    bricks[i][j].draw();
                }
            }
        }
    }
    switch (hit){
        case 0:
            break;
        case 1:
            // ball1.angle = ball1.angle + 180;
            ball1.angle = -1*ball1.angle;
            break;
        case 2:
            //ball2.angle = ball2.angle + 180;
            ball2.angle = -1*ball2.angle;
            break;
    }
    
}

void myKeyboard(unsigned char thekey,int mouseX,int mouseY){
    switch(thekey){
        case 'h':
            pad1.center -= 5;
            break;
        case 'l':
            pad1.center += 5;
            break;
            
    }
    glutPostRedisplay();
}

void myMouse(int x, int y){
    pad2.center = x-300;
}

void anim(){
    if (!gameOver()){
        for (int i = 0; i <1e2; i++){ }
        secs+=0.01f;
        ball1.applySpeed();
        ball2.applySpeed();
        
        if (pad2.isHit(ball1)){
            ball1.angle = 90 - 90.0 * (ball1.x-(pad2.center))/pad2.width;
        }else if (pad1.isHit(ball1)){
            ball1.angle = 90 - 90.0 * (ball1.x-(pad1.center))/pad1.width;
        }
        
        if (pad2.isHit(ball2)){
            ball2.angle = 90 - 90.0 * (ball2.x-(pad2.center))/pad2.width;
        }else if (pad1.isHit(ball2)){
            ball2.angle = 90 - 90.0 * (ball2.x-(pad1.center))/pad1.width;
        }
        glutPostRedisplay();
    }
}
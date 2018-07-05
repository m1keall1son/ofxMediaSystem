# ofxMediaSystem Overview
ofxMediaSystem is a flexible data structure for generically composing data and logic in openFrameworks applications that require many different states of operation.

## Installation
there are no other dependencies.  Just use as indicated by the Wiki

```c++
using namespace mediasystem;
SceneManager mSceneManager;
...
//create new scene
std::shared_ptr<Scene> scene = mSceneManager.createScene<Scene>("myScene");

//create an empty entity
EntityHandle entityHandle = scene->createEntity();

//entities and their components are accessed through weak_ptr's
//the user should not store the contained shared_ptrs directly
auto entity = entityHandle.lock();

//entities have an interface for scene graphing and 3D transformations
entity->setPosition(windowCenter.x, windowCenter.y, 0.f);

//define a new component
struct SomeData {
SomeData(float _speed):hitCount(0),speed(_speed){}
int hitCount;
float speed;
};

float speed = 10.f;
//add a data component to the entity, this returns a handle to access that data
std::weak_ptr<SomeData> componentHandle = entity->createComponent<SomeData>(speed);

//there's a default method for drawing things to the screen by creating a LayeredRenderer system.
scene->createSystem<LayeredRenderer>(*scene);

//the LayeredRenderer will draw component wrapped in a Drawable<typename T> template.
//the template argument must fit the drawable concept by having a "void draw()" method
struct DrawText {
DrawText(std::string m, glm::vec2 pos):message(std::move(m)),position(std::move(pos)){}
std::string message;
glm::vec2 position;
void draw(){
ofDrawBitMappedString(message, position.x, position.y);
}
};

//lets have this component put a message on screen
entity->createComponent<mediasystem::Drawable<DrawText>>("Hello World", glm::vec2(10,10));

mSceneManager.initScenes(); //will initialize ALL scenes
mSceneManager.changeSceneTo(scene); //will cause scene to 'transition in'
...
//will emit update event in the current scene
//a system could tie into that event to update the state of SomeData components
mSceneManager.update();

...
//will emit draw event in the current scene, which the LayeredRenderer will use to draw our text to screen
mSceneManager.draw();
```

#include <vector>
#include <SFML/Graphics.hpp>

#include "Box2D/Collision/Shapes/b2PolygonShape.h"
#include "Box2D/Common/b2Math.h"
#include "Box2D/Dynamics/b2Fixture.h"
#include "Box2D/Dynamics/b2World.h"

using namespace std;
using namespace sf;

// 1 sfml unit = 30 physics units
const float physics_scale = 30.0f;
// inverse of physics_scale, useful for calculations
const float physics_scale_inv = 1.0f / physics_scale;
// Magic numbers for accuracy of physics simulation
const int32 velocityIterations = 6;
const int32 positionIterations = 2;

// Game window dimensions
const int gameWidth = 800;
const int gameHeight = 600;

// Function prototypes
void init();
void Update();
void cleanup();

// Convert from b2Vec2 to a Vector2f
inline const Vector2f bv2_to_sv2(const b2Vec2& in) {
    return Vector2f(in.x * physics_scale, (in.y * physics_scale));
}

// Convert from Vector2f to a b2Vec2
inline const b2Vec2 sv2_to_bv2(const Vector2f& in) {
    return b2Vec2(in.x * physics_scale_inv, in.y * physics_scale_inv);
}

// Invert height for screen-space coordinate
inline const Vector2f invert_height(const Vector2f& in) {
    return Vector2f(in.x, gameHeight - in.y);
}

// Create a Box2D body with a box fixture
b2Body* CreatePhysicsBox(b2World& World, const bool dynamic, const Vector2f& position, const Vector2f& size) {
    b2BodyDef BodyDef;
    BodyDef.type = dynamic ? b2_dynamicBody : b2_staticBody;
    BodyDef.position = sv2_to_bv2(position);
    b2Body* body = World.CreateBody(&BodyDef);

    b2PolygonShape Shape;
    Shape.SetAsBox(sv2_to_bv2(size).x * 0.5f, sv2_to_bv2(size).y * 0.5f);

    b2FixtureDef FixtureDef;
    FixtureDef.density = dynamic ? 10.f : 0.f;
    FixtureDef.friction = dynamic ? 0.8f : 1.f;
    FixtureDef.restitution = 1.0;
    FixtureDef.shape = &Shape;
    body->CreateFixture(&FixtureDef);

    return body;
}

// Create a Box2D body with a box fixture from a RectangleShape
b2Body* CreatePhysicsBox(b2World& world, const bool dynamic, const RectangleShape& rs) {
    return CreatePhysicsBox(world, dynamic, rs.getPosition(), rs.getSize());
}

// Global variables
b2World* world;
std::vector<b2Body*> bodies;
std::vector<RectangleShape*> sprites;
void init() {
    const b2Vec2 gravity(0.0f, -10.0f);
    world = new b2World(gravity);

    // Wall Dimensions
    Vector2f walls[] = {
        // Top
        Vector2f(gameWidth * .5f, 5.f), Vector2f(gameWidth, 10.f),
        // Bottom
        Vector2f(gameWidth * .5f, gameHeight - 5.f), Vector2f(gameWidth, 10.f),
        // Left
        Vector2f(5.f, gameHeight * .5f), Vector2f(10.f, gameHeight),
        // Right
        Vector2f(gameWidth - 5.f, gameHeight * .5f), Vector2f(10.f, gameHeight)
    };

    // Build Walls
    for (int i = 0; i < 8; i += 2) {
        // Create SFML shapes for each wall
        auto s = new RectangleShape();
        s->setPosition(walls[i]);
        s->setSize(walls[i + 1]);
        s->setOrigin(walls[i + 1] * 0.5f);
        sprites.push_back(s);

        // Create a static physics body for the wall
        auto b = CreatePhysicsBox(*world, false, *s);
        bodies.push_back(b);
    }

    // Create Boxes
    for (int i = 1; i < 11; ++i) {
        RectangleShape* s = new RectangleShape();
        s->setPosition(Vector2f(i * (gameWidth / 12.f), gameHeight * .7f));
        s->setSize(Vector2f(50.0f, 50.0f));
        s->setOrigin(Vector2f(25.0f, 25.0f));
        s->setFillColor(Color::White);
        sprites.push_back(s);

        b2Body* b = CreatePhysicsBox(*world, true, *s);
        b->ApplyAngularImpulse(5.0f, true);
        bodies.push_back(b);
    }
}

void Update() {
    static sf::Clock clock;
    float dt = clock.restart().asSeconds();
    world->Step(dt, velocityIterations, positionIterations);

    for (int i = 0; i < bodies.size(); ++i) {
        sprites[i]->setPosition(invert_height(bv2_to_sv2(bodies[i]->GetPosition())));
        sprites[i]->setRotation((180 / b2_pi) * bodies[i]->GetAngle());
    }
}

void cleanup() {
    delete world;
    for (auto sprite : sprites) {
        delete sprite;
    }
}

int main() {
    RenderWindow window(VideoMode(gameWidth, gameHeight), "Box2D with SFML");

    init();
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }
        Update();
        window.clear(Color::Black);
        for (const auto& sprite : sprites) {
            window.draw(*sprite);
        }
        window.display();
    }

    cleanup();
    return 0;
}

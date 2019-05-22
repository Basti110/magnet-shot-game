#include "mesh_node.h"
#include "load_mesh.h"
#include "bullet_helper.hh"

#include <polymesh/Mesh.hh>
#include <polymesh/algorithms/properties.hh>
#include <polymesh/fields.hh>
#include <polymesh/formats/obj.hh>

#include <glow/objects/VertexArray.hh>


MeshNode::MeshNode(const glm::mat4& transform, const std::string& filename, PhysicsManager* physics, short collisionGroup, short collisionMask, float mass) :
    PhysicsNode(physics)
{
    mLocalTransformation = transform;
    mGlobalTransformation = transform;

    const size_t idx = filename.find_last_of(".");
    const std::string collisionFilename = filename.substr(0, idx) + "Collision.obj";

    // collision mesh
    pm::Mesh polymesh;
    pm::obj_reader<float> obj_reader(collisionFilename, polymesh);
    auto pos = obj_reader.get_positions().map([](std::array<float, 4> const& t) {
            return btVector3(t[0], t[1], t[2]);
    });
    btDefaultMotionState* motionState = new btDefaultMotionState(to_bullet(transform));

    if (mass > 0) {
        btConvexHullShape* collisionShape = new btConvexHullShape();
        for (auto v : polymesh.vertices()) {
            collisionShape->addPoint(pos[v]);
        }
        btVector3 inertia;
        collisionShape->calculateLocalInertia(mass, inertia);
        auto btInfo = btRigidBody::btRigidBodyConstructionInfo(mass, motionState, collisionShape, inertia);
        // btInfo.m_friction = info.friction;
        // btInfo.m_restitution = info.restitution;
        // btInfo.m_linearDamping = info.linearDamping;
        // btInfo.m_angularDamping = info.angularDamping;
        mRigidBody = new btRigidBody(btInfo);
    }
    else {
        btTriangleMesh* triangleMesh = new btTriangleMesh();
        for (auto f : polymesh.faces()) {
            std::vector<btVector3> verts;
            for (auto v : f.vertices()) {
                verts.push_back(pos[v]);
            }
            triangleMesh->addTriangle(verts[0], verts[1], verts[2]);
        }
        btCollisionShape* collisionShape = new btBvhTriangleMeshShape(triangleMesh, true);
        mRigidBody = new btRigidBody(mass, motionState, collisionShape);
    }
    mWorld->addRigidBody(mRigidBody, collisionGroup, collisionMask);

    // visual mesh
    mVertexArray = load_mesh_from_obj(filename, false);
}

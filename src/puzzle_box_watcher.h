#pragma once
#include "puzzle_box.h"


class PuzzleBoxWatcher {
public:
    PuzzleBoxWatcher() : mIsActive(true) { }

    void addPuzzleBox(PuzzleBox* box, const glm::vec3& correctPosition)
    {
        mBodies.push_back(box->getRigidBody());
        mCorrectPositions.push_back(correctPosition);
    }

    void update()
    {
        if (mIsActive) {
            for (size_t i = 0; i < mBodies.size(); i++) {
                const glm::vec3 p = to_glm(mBodies[i]->getCenterOfMassPosition());
                if (glm::length(p - mCorrectPositions[i]) > 0.025) {
                    return;
                }
            }
            MessageBus::getInstance()->sendMessage(new ActivateScreenMessage(0));
            mIsActive = false;
        }
    }

private:
    std::vector<btRigidBody*> mBodies;
    std::vector<glm::vec3> mCorrectPositions;
    bool mIsActive;
};

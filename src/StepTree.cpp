#include "StepTree.h"
#include "Field.h"
#include "Piece.h"

#include <algorithm>

StepTree::StepTree() : parent(nullptr), field(nullptr), capture(nullptr), capturesNumber(0U), isValid(false) {}

void StepTree::evalCapturesNumber()
{
    uint16_t captureIncr = 0U;
    if (capture != nullptr)
        captureIncr = 1U;
    capturesNumber += captureIncr;
    for (StepTree *subStep : nextStepList) {
        subStep->capturesNumber = capturesNumber;
        subStep->evalCapturesNumber();
    }
    for (StepTree *subStep : nextStepList) {
        capturesNumber = std::max(capturesNumber, subStep->capturesNumber);
    }
}

void StepTree::validate(uint16_t capturesNumber)
{
    isValid = (capturesNumber == this->capturesNumber);
    if (parent == nullptr)
        isValid = (isValid && !nextStepList.empty());
    for (StepTree *subStep : nextStepList) {
        subStep->validate(capturesNumber);
    }
}

StepTree::~StepTree()
{
    for (StepTree *subStep : nextStepList) {
        delete subStep;
    }
}
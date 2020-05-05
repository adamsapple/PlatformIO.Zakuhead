#include <Arduino.h>
#include <keyframe.h>

/**
 * 
 * 
 */
KeyFrameAnimator::KeyFrameAnimator():
    IsPlayng(false), now(0), nowValue(0), intMap(0, 1, 0, 1)
{
    //ctor
    targetKeyFrame = NULL;
}

KeyFrameAnimator::~KeyFrameAnimator()
{
    //dtor
}

void KeyFrameAnimator::Start()
{
    IsPlayng = true;
}

void KeyFrameAnimator::Start(int value)
{
    StartValue = value;
    nowValue   = value;
    Start();
}


void KeyFrameAnimator::Stop()
{
    IsPlayng = false;
}

/*
void KeyFrameAnimator::Update()
{
    if(!IsPlayng)
    {
        return;
    }

    now = millis();

    if(targetKeyFrame == NULL)
    {
        Finished(this, NULL);
        return;
    }

    nowValue = (StartTimeMS == EndTimeMS)?EndValue:map(now, StartTimeMS, EndTimeMS, StartValue, EndValue);
    
    // Serial.print("nw: ");       Serial.print(now);
    // Serial.print("   st: ");    Serial.print(StartTimeMS);
    // Serial.print("   et: ");    Serial.print(EndTimeMS);
    // Serial.print("   sv: ");    Serial.print(StartValue);
    // Serial.print("   ev: ");    Serial.print(EndValue);

    // Serial.print("   nv: ");    Serial.println(nowValue);

    if(now >= EndTimeMS)
    {
        if(++nowFrameId < numFrames){
            SetKeyFrameImplById(nowFrameId);
        }else{
            targetKeyFrame = NULL;
            Finished(this, NULL);
        }
    }
}
*/

#ifndef __KEYFRAME_H__
#define __KEYFRAME_H__

#include <event.h>
#include <intmap.h>

enum {
    FRAME_NONE,
    FRAME_SERVO_FREEZE,
    FRAME_SERVO_MOVE,
    FRAME_LED_OFF,      // 消灯
    FRAME_LED_BRIGHT,   // 明るい
    FRAME_LED_DARK,     // 暗い
    FRAME_LED_BLINK,    // 点滅3回(1-0-1-0-1)
} FRAMETYPE;

typedef struct {
    unsigned int  TimeSpanMS;
    uint8_t type;
    int  DestValue;
    bool IsFreeze;
} KeyFrame;

/**
 * 
 * 
 */
class KeyFrameAnimator
{
    public:
        KeyFrameAnimator();
        virtual ~KeyFrameAnimator();

        void Start();
        void Start(int value);
        void Stop();
        void Update(unsigned int n);
        
        int GetValue() const;
        int GetStartValue() const {return StartValue;} 
        int GetEndValue() const{return EndValue;}
        void SetKeyFrame(KeyFrame& keyFrame);
        void SetKeyFrames(KeyFrame* keyFrames, int num);
        Event<EventFunc> Finished;

    protected:
        bool IsPlayng;
        unsigned int StartTimeMS;
        unsigned int EndTimeMS;
        int StartValue;
        int EndValue;
        KeyFrame* targetKeyFrame;
        
        void SetKeyFrameImplById(int frameId);
        void SetKeyFrameImpl(KeyFrame& keyFrame);

    private:
        unsigned int now;
        int nowValue;
        int numFrames;
        int nowFrameId;
        TimeMillisMap intMap;
        //IntMap intMap;
};

inline void KeyFrameAnimator::Update(unsigned int n)
{
    if(!IsPlayng)
    {
        return;
    }

    now = n;//millis();

    if(targetKeyFrame == NULL)
    {
        Finished(this, NULL);
        return;
    }

    //nowValue = (StartTimeMS == EndTimeMS)?EndValue:map(now, StartTimeMS, EndTimeMS, StartValue, EndValue);
    if (StartTimeMS > EndTimeMS) {
        return;
    }

    nowValue = intMap.Constrain(now);
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



inline int KeyFrameAnimator::GetValue() const
{
    //nowValue = map(now, StartTimeMS, EndTimeMS, StartValue, EndValue);
    //Serial.print("nowValue: ");
    //Serial.print(nowValue);
    return nowValue;
}

inline void KeyFrameAnimator::SetKeyFrame(KeyFrame& keyFrame)
{
    nowFrameId     = 0;
    targetKeyFrame = &keyFrame;
    numFrames      = 1;

    SetKeyFrameImpl(keyFrame);
}

inline void KeyFrameAnimator::SetKeyFrames(KeyFrame* pkeyFrame, int num)
{
    nowFrameId     = 0;
    targetKeyFrame = pkeyFrame;
    numFrames      = num;

    SetKeyFrameImplById(nowFrameId);
}

inline void KeyFrameAnimator::SetKeyFrameImplById(int frameId)
{
    KeyFrame& keyFrame = targetKeyFrame[frameId];
    SetKeyFrameImpl(keyFrame);
}

inline void KeyFrameAnimator::SetKeyFrameImpl(KeyFrame& keyFrame)
{
    StartTimeMS = now;
    EndTimeMS   = StartTimeMS + keyFrame.TimeSpanMS;
    StartValue  = nowValue;
    EndValue    = (keyFrame.IsFreeze)?StartValue:keyFrame.DestValue;

    intMap.Reset(StartTimeMS, EndTimeMS, StartValue, EndValue);
    // Serial.print("st: ");
    // Serial.print(StartTimeMS);
    // Serial.print("  et: ");
    // Serial.print(EndTimeMS);
    // Serial.print("  sv: ");
    // Serial.print(StartValue);
    // Serial.print("  dv: ");
    // Serial.println(EndValue);
}
#endif // __KEYFRAME_H__

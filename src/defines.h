#pragma once
////
/// value settings.
//
const int minPulse    = 500;
const int maxPulse    = 2400;
const int minAngle    = 0;
const int centerAngle = 90;
const int maxAngle    = 180;
const int leftAngle   = minAngle + 55;
const int rightAngle  = maxAngle - 55;

const int brightDuty  = 255;
const int defaultDuty = 72;
const int darkDuty    = 7;
const int offDuty     = 0;

////
/// pin settings.
//
const int ledPin   = 3;
const int servoPin = 9;
const int voicePin = 11;//3;
const int spkEnablePin = 7;

////
/// Led Animations.
//
enum LedMotion {
    LED_BOOT,
    LED_BLINK,
    LED_KEEP,
    LED_OFF,
    LED_PATTERN_NUM
};

const KeyFrame ledKeyFrames_OFF[] = {
    {500, 0, offDuty, false},
    {500, 0, 0,        true},
};

const KeyFrame ledKeyFrames_Boot[] = {
    {0,    0, offDuty,     false},
    {300,  0, brightDuty,  false},
    {500,  0, 0,           true},
    {300,  0, defaultDuty, false},
    {1000, 0, 0,           true},
};

const KeyFrame ledKeyFrames_LED_BLINK[] = {
    {20, 0, offDuty,     false}, {105, 0, 0, true},
    {20, 0, defaultDuty, false}, {105, 0, 0, true},
    {20, 0, offDuty,     false}, {105, 0, 0, true},
    {20, 0, defaultDuty, false}, {105, 0, 0, true},
    {20, 0, offDuty,     false}, {105, 0, 0, true},
    {20, 0, defaultDuty, false}, {605, 0, 0, true},
};

const KeyFrame ledKeyFrames_KEEP[] = {
    {1000, 0, 0, true},
};
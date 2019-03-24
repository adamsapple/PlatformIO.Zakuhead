#ifndef INTMAP_H_
#define INTMAP_H_

#include <Arduino.h>

/** A faster version of Arduino's map() function.
 * This uses ints instead of longs internally and does some of the maths at compile time.
 * https://github.com/sensorium/Mozzi/blob/master/IntMap.h
*/
class IntMap {

public:
	/** Constructor.
	@param imin the minimum of the input range.
	@param imax the maximum of the input range.
	@param omin the minimum of the output range.
	@param omax the maximum of the output range.
	*/
	IntMap(int imin, int imax, int omin, int omax)
			: inMin(imin), inMax(imax), outMin(omin), outMax(omax)
	{
		if (imin != imax) {
            mul = (256L * (omax-omin)) / (imax-imin);
        };
	}

    void Reset(int imin, int imax, int omin, int omax)
	{
		inMin  = imin;
        inMax  = imax;
        outMin = omin;
        outMax = omax;
        mul    = 0;
        if (imin != imax) {
            mul = (256L * (omax-omin)) / (imax-imin);
        };
    }

	/** Process the next input value.
	 * @param v the next integer to process.
	 * @return the input integer mapped to the output range.
	*/
	int Value(int v) const {
        return (int)(((mul * (v - inMin))>>8) + outMin);
	}

    /**
     * 
     * 
     */
    int Constrain(int v) const {
        v = constrain(v, inMin, inMax);
        return Value(v);
    }
/*
    // map.
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
*/

private:
	int inMin, inMax, outMin, outMax;
	long mul;
};

#endif /* INTMAP_H_ */
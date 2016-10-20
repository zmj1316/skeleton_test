#include "mathUtil.h"

namespace MathUtil
{
	float bezier(float X, float x1, float y1, float x2, float y2)
	{
		//The below function approximates the Y of a bezier curve for a given X using the bisection method.
		//...This is technically the bisection method, but I wrote the code myself, so it may not be very optimized.
		//It's also well known that there are faster methods of solving bezier curves, such as the Newton method.
		//However, bisection is a simpler algorithm that works well for MMD's curves, who work strictly in the range of [0,1]

		x1 /= 127.0f;
		x2 /= 127.0f;
		y1 /= 127.0f;
		y2 /= 127.0f;

		float x0, y0, x3, y3;
		x0 = y0 = 0;
		x3 = y3 = 1;


		//float t=0.41f;

		//3*((1-B8)^2)*B8*$B$4+3*(1-B8)*(B8^2)*$B$5+B8^3

		//For example problem, Y=0.0716009

		float lowTGuess = 0.0f;
		float dividingT = 0.5f;
		float highTGuess = 1.0f;

		float lowXCalc;
		float divideXCalc;

		for (int i = 0; i < 100; ++i)
		{
			lowXCalc = X - parBezFunc(lowTGuess, x1, x2);
			divideXCalc = X - parBezFunc(dividingT, x1, x2);

			if (abs(divideXCalc) < 0.0001)
			{
				//cout<<"XCalc close to 0, breaking"<<endl;
				break;
			}

			if ((lowXCalc * divideXCalc)<0)
			{
				//cout<<"signs different"<<endl;
				highTGuess = dividingT;
				dividingT = (dividingT + lowTGuess) / 2.0f;
			}
			else
			{
				//cout<<"signs same"<<endl;
				lowTGuess = dividingT;
				//cout<<"("<<highTGuess<<"+"<<dividingT<<")"<<"/2.0f"<<endl;
				dividingT = (highTGuess + dividingT) / 2.0f;
			}
		}

		return parBezFunc(dividingT, y1, y2);

		//cout<<lowXCalc<<" "<<highXCalc<<" "<<dotProduct<<endl;
		//cout<<glm::dot(5.0,5.0)<<" "<<glm::dot(-5.0,5.0)<<" "<<glm::dot(5.0,-5.0)<<endl;


		//float xT=x0 + (3*t)*(x1) + (3*t*t)*(x2-2*x1) + (t*t*t)*(x3+3*x1-3*x2);
		//float yT=y0 + (3*t)*(y1-y0) + (3*t*t)*(y0+y2-2*y1) + (t*t*t)*(y3-y0+3*y1-3*y2);

		return 0.0f;
	}
}

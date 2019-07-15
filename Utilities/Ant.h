/*
 * Ant.h
 *
 *  Created on: 4 lug 2019
 *      Author: gb1609
 */

#ifndef ANT_H_
#define ANT_H_

class Ant
{

private:
	bool food=false;
  public:
    Ant ();

	bool isFood() const {
		return food;
	}

	void setFood(bool food = false) {
		this->food = food;
	}
};

#endif /* ANT_H_ */

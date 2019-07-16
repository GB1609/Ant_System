/*
 * Ant.h
 *
 *  Created on: 4 lug 2019
 *      Author: gb1609
 */

#ifndef ANT_H_
#define ANT_H_

class Ant {

private:
	bool food;
	int toIncreasePheromon;
	int currentPosition;
	vector<Direction> path;
public:
	Ant(int pos, int toI) {
		toIncreasePheromon = toI;
		food = false;
		currentPosition = pos;
	}

	bool isFood() const {
		return food;
	}

	void setFood(bool food) {
		this->food = food;
	}

	void addMove(Direction a) {
		Direction toPut;
		switch (a) {
		case LEFT:
			toPut = RIGHT;
			break;
		case RIGHT:
			toPut = LEFT;
			break;
		case DOWN:
			toPut = UP;
			break;
		case UP:
			toPut = DOWN;
			break;
		case DOWN_RIGHT:
			toPut = UP_LEFT;
			break;
		case UP_RIGHT:
			toPut = DOWN_LEFT;
			break;
		case UP_LEFT:
			toPut = DOWN_RIGHT;
			break;
		case DOWN_LEFT:
			toPut = UP_RIGHT;
			break;
		default:
			cout << "ERRORE in ADD MOVE" << endl;
			toPut = BORN;
		}
		path.push_back(toPut);
	}

	Direction getLastDirection() {
		if (path.empty())
			cout << "path in ant emptu" << endl;
		return path.back();
	}

	void remove_move() {
		path.pop_back();
	}

	int getCurrentPosition() const {
		return currentPosition;
	}

	void setCurrentPosition(int currentPosition) {
		this->currentPosition = currentPosition;
	}

	void setPath(const vector<Direction> &path) {
		this->path = path;
	}

	int getToIncreasePheromon() const {
		return toIncreasePheromon;
	}

	void decreaseToIncreasePheromon() {
		(toIncreasePheromon > 0) ? toIncreasePheromon-=5 : toIncreasePheromon;
	}
};

#endif /* ANT_H_ */

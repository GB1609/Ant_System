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
	int mySource;
	int toIncreasePheromon;
	int currentPosition;
	int x;
	int y;
	int pass_to_find = 0;
	int pass_to_come_back = 0;
	vector<Direction> path;
public:
	Ant(int pos, int toI,int x, int y) {
		toIncreasePheromon = toI;
		food = false;
		currentPosition = pos;
		mySource = pos;
		this->x=x;
		this->y=y;
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

	void setCurrentPosition(int currentPosition,int x, int y) {
		this->currentPosition = currentPosition;
		this->x=x;
		this->y=y;
	}

	void setPath(const vector<Direction> &path) {
		this->path = path;
	}

	int getToIncreasePheromon() const {
		return toIncreasePheromon;
	}

	void decreaseToIncreasePheromon() {
		if (toIncreasePheromon > 2)
			toIncreasePheromon -= 1;
	}

	int getPassToComeBack() const {
		return pass_to_come_back;
	}

	void update_pass_to_come_back() {
		pass_to_come_back++;
	}

	int getPassToFind() const {
		return pass_to_find;
	}

	void update_pass_to_find() {
		pass_to_find++;
	}

	const vector<Direction>& getPath() const {
		return path;
	}

	void setToIncreasePheromon(int toIncreasePheromon) {
		this->toIncreasePheromon = toIncreasePheromon;
	}

	int getMySource() const {
		return mySource;
	}

	void setMySource(int mySource) {
		this->mySource = mySource;
	}

	void setPassToComeBack(int passToComeBack = 0) {
		pass_to_come_back = passToComeBack;
	}

	void setPassToFind(int passToFind = 0) {
		pass_to_find = passToFind;
	}

	int getX() const {
		return x;
	}

	int getY() const {
		return y;
	}
};

#endif /* ANT_H_ */

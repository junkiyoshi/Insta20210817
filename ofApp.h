#pragma once
#include "ofMain.h"

class Actor {
public:
	Actor(vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list);
	Actor(int select_index, vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list);
	void update(const int& frame_span, vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list);
	glm::vec3 getLocation();
	deque<glm::vec3> getLog();

private:

	int select_index;
	int next_index;

	glm::vec3 location;
	deque<glm::vec3> log;
};

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key) {
		
		if (key == 'r' && key == 'R') {

			this->rec_flag = true;
		}
	};
	void keyReleased(int key) {};
	void mouseMoved(int x, int y) {};
	void mouseDragged(int x, int y, int button) {};
	void mousePressed(int x, int y, int button) {};
	void mouseReleased(int x, int y, int button) {};
	void windowResized(int w, int h) {};
	void dragEvent(ofDragInfo dragInfo) {};
	void gotMessage(ofMessage msg) {};

	bool rec_flag = false;

	ofTrueTypeFont font;
	vector<string> word_list;
	int word_index;

	vector<glm::vec3> location_list;
	vector<vector<int>> next_index_list;
	vector<int> destination_list;

	vector<unique_ptr<Actor>> actor_list;

	ofEasyCam cam;
};
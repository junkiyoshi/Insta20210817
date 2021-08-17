#include "ofApp.h"	

//--------------------------------------------------------------
Actor::Actor(vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list) {

	this->select_index = ofRandom(location_list.size());
	while (true) {

		auto itr = find(destination_list.begin(), destination_list.end(), this->select_index);
		if (itr == destination_list.end()) {

			destination_list.push_back(this->select_index);
			break;
		}

		this->select_index = (this->select_index + 1) % location_list.size();
	}

	this->next_index = this->select_index;
}

//--------------------------------------------------------------
Actor::Actor(int select_index, vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list) {

	this->select_index = select_index;
	while (true) {

		auto itr = find(destination_list.begin(), destination_list.end(), this->select_index);
		if (itr == destination_list.end()) {

			destination_list.push_back(this->select_index);
			break;
		}

		this->select_index = (this->select_index + 1) % location_list.size();
	}

	this->next_index = this->select_index;

	this->location = location_list[this->select_index];
}

//--------------------------------------------------------------
void Actor::update(const int& frame_span, vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list) {

	if (ofGetFrameNum() % frame_span == 0) {

		auto tmp_index = this->select_index;
		this->select_index = this->next_index;
		int retry = next_index_list[this->select_index].size();
		this->next_index = next_index_list[this->select_index][(int)ofRandom(next_index_list[this->select_index].size())];
		while (--retry > 0) {

			auto destination_itr = find(destination_list.begin(), destination_list.end(), this->next_index);
			if (destination_itr == destination_list.end()) {

				if (tmp_index != this->next_index) {

					destination_list.push_back(this->next_index);
					break;
				}
			}

			this->next_index = next_index_list[this->select_index][(this->next_index + 1) % next_index_list[this->select_index].size()];
		}
		if (retry <= 0) {

			destination_list.push_back(this->select_index);
			this->next_index = this->select_index;
		}
	}

	auto param = ofGetFrameNum() % frame_span;
	auto distance = location_list[this->next_index] - location_list[this->select_index];
	this->location = location_list[this->select_index] + distance / frame_span * param;

	this->log.push_front(this->location);
	while (this->log.size() > 50) { this->log.pop_back(); }
}

//--------------------------------------------------------------
glm::vec3 Actor::getLocation() {

	return this->location;
}

//--------------------------------------------------------------
deque<glm::vec3> Actor::getLog() {

	return this->log;
}

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openFrameworks");

	ofBackground(239);
	ofSetLineWidth(2);
	ofEnableDepthTest();

	this->font.loadFont("fonts/Kazesawa-Bold.ttf", 350, true, true, true);

	this->word_list = vector<string>{ "S", "&", "B" };
	this->word_index = 0;
}

//--------------------------------------------------------------
void ofApp::update() {

	if (ofGetFrameNum() % 120 == 0) {

		this->location_list.clear();
		this->next_index_list.clear();
		this->destination_list.clear();
		this->actor_list.clear();

		auto span = 8;

		for (int x = span * 10; x < ofGetWidth() - span * 10; x += span) {

			for (int y = span * 10; y < ofGetHeight() - span * 10; y += span) {

				for (int z = -300; z <= 300; z += 300) {

					this->location_list.push_back(glm::vec3(x, y, z));
				}
			}
		}

		for (auto& location : this->location_list) {

			vector<int> next_index = vector<int>();
			int index = -1;
			for (auto& other : this->location_list) {

				index++;
				if (location == other) { continue; }

				float distance = glm::distance(location, other);
				if (distance <= span) {

					if (location.x > ofGetWidth() * 0.5 && location.x <= other.x) {

						if (location.y > ofGetHeight() * 0.5 && location.y <= other.y) {

							next_index.push_back(index);
						}

						if (location.y <= ofGetHeight() * 0.5 && location.y >= other.y) {

							next_index.push_back(index);
						}
					}

					if (location.x <= ofGetWidth() * 0.5 && location.x >= other.x) {

						if (location.y > ofGetHeight() * 0.5 && location.y <= other.y) {

							next_index.push_back(index);
						}

						if (location.y <= ofGetHeight() * 0.5 && location.y >= other.y) {

							next_index.push_back(index);
						}
					}
				}
			}

			this->next_index_list.push_back(next_index);
		}

		for (int z = -300; z <= 300; z += 300) {

			ofFbo fbo;
			fbo.allocate(ofGetWidth(), ofGetHeight());
			fbo.begin();
			ofTranslate(ofGetWidth() * 0.5, ofGetHeight() * 0.5);
			ofClear(0);
			ofSetColor(0);

			string word = this->word_list[this->word_index];
			this->word_index = (this->word_index + 1) % this->word_list.size();
			this->font.drawString(word, this->font.stringWidth(word) * -0.5, this->font.stringHeight(word) - 150);

			fbo.end();

			ofPixels pixels;
			fbo.readToPixels(pixels);
			for (int x = 0; x < fbo.getWidth(); x += span) {

				for (int y = 0; y < fbo.getHeight(); y += span) {

					ofColor color = pixels.getColor(x, y);
					if (color != ofColor(0, 0)) {

						for (int i = 0; i < this->location_list.size(); i++) {

							if (this->location_list[i] == glm::vec3(x, y, z)) {

								this->actor_list.push_back(make_unique<Actor>(i, this->location_list, this->next_index_list, this->destination_list));
								break;
							}
						}
					}
				}
			}
		}
	}

	if (ofGetFrameNum() % 120 < 15) {

		return;
	}

	int frame_span = 1;
	int prev_index_size = 0;

	if (ofGetFrameNum() % frame_span == 0) {

		prev_index_size = this->destination_list.size();
	}

	for (auto& actor : this->actor_list) {

		actor->update(frame_span, this->location_list, this->next_index_list, this->destination_list);
	}

	if (prev_index_size != 0) {

		this->destination_list.erase(this->destination_list.begin(), this->destination_list.begin() + prev_index_size);
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->cam.begin();
	ofRotateX(180);
	ofTranslate(ofGetWidth() * -0.5, ofGetHeight() * -0.5, 300);	

	for (auto& actor : this->actor_list) {

		ofSetColor(39);
		ofFill();
		ofDrawBox(actor->getLocation(), 8);

		ofSetColor(239);
		ofNoFill();
		ofDrawBox(actor->getLocation(), 8);
	}

	this->cam.end();
}


//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}
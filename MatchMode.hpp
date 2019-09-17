#include "Mode.hpp"
#include "Sound.hpp"

#include <unordered_map>
#include <vector>
#include <string>
#include <array>
#include <cmath>
#include <functional>
#include <random>
#include <algorithm>
#include <iostream>

struct MatchMode : Mode {
	MatchMode();
	virtual ~MatchMode();

	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	glm::vec2 view_min = glm::vec2(0,0);
	glm::vec2 view_max = glm::vec2(640, 480);

  std::unordered_map< SDL_Keycode, Sound::Sample const * > cmap;

  bool curr_held_active = false;
  SDL_Keycode curr_held = SDLK_a;

  uint32_t wrong_count = 0;
  uint32_t correct_count = 0;

  float countdown = 5.0f;
  bool countdown_start = false;

  bool remain_getting = false;
  std::vector< SDL_Keycode >::const_iterator remain_it;
  std::shared_ptr< Sound::PlayingSample > remain_curr_sound;

};

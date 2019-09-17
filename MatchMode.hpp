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
#include <chrono>

struct MatchMode : Mode {
	MatchMode();
	virtual ~MatchMode();

	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

  std::unordered_map< SDL_Keycode, Sound::Sample const * > cmap;

  bool curr_held_active = false;
  SDL_Keycode curr_held = SDLK_a;

  float countdown = 15.0f;
  uint32_t countdown_next_ping = 4;
  bool countdown_start = false;

  bool remain_getting = false;
  std::vector< SDL_Keycode >::const_iterator remain_it;
  std::shared_ptr< Sound::PlayingSample > remain_curr_sound;

  bool finish = false;
  std::shared_ptr< Sound::PlayingSample > finish_sound;
  uint32_t try_count = 0;
  uint32_t try_count_digits = 0;
  uint32_t correct_count = 0;

};

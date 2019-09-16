#include "MatchMode.hpp"

#include "Sprite.hpp"
#include "DrawSprites.hpp"
#include "Load.hpp"
#include "data_path.hpp"
#include "gl_errors.hpp"
#include "Sound.hpp"

static const uint32_t COUNT_PAIRS = 13;

Load< std::vector< SDL_Keycode > > keys(
  LoadTagLate, []() -> std::vector< SDL_Keycode > const * {
	std::vector< SDL_Keycode > *ret =
    new std::vector< SDL_Keycode >();

	ret->emplace_back(SDLK_a);
  ret->emplace_back(SDLK_b);
  ret->emplace_back(SDLK_c);
  ret->emplace_back(SDLK_d);
  ret->emplace_back(SDLK_e);
  ret->emplace_back(SDLK_f);
  ret->emplace_back(SDLK_g);
  ret->emplace_back(SDLK_h);
  ret->emplace_back(SDLK_i);
  ret->emplace_back(SDLK_j);
  ret->emplace_back(SDLK_k);
  ret->emplace_back(SDLK_l);
  ret->emplace_back(SDLK_m);
  ret->emplace_back(SDLK_n);
  ret->emplace_back(SDLK_o);
  ret->emplace_back(SDLK_p);
  ret->emplace_back(SDLK_q);
  ret->emplace_back(SDLK_r);
  ret->emplace_back(SDLK_s);
  ret->emplace_back(SDLK_t);
  ret->emplace_back(SDLK_u);
  ret->emplace_back(SDLK_v);
  ret->emplace_back(SDLK_w);
  ret->emplace_back(SDLK_x);
  ret->emplace_back(SDLK_y);
  ret->emplace_back(SDLK_z);

  assert(ret->size() == COUNT_PAIRS * 2);

	return ret;

});

Load< std::vector< Sound::Sample *> const > sounds(
  LoadTagLate, []() -> std::vector< Sound::Sample *> const * {
	std::vector< Sound::Sample *> *ret =
    new std::vector< Sound::Sample * >();

  std::array< float, 7 > const freqs = {
    261.63f,
    293.66f,
    329.63f,
    349.23f,
    392.00f,
    440.00f,
    493.88f
  };

  // 1/duration
  float const dur_time = 0.25f;
  uint32_t const dur_samples = (uint32_t) (Sound::SRATE * dur_time);

  auto get_env = [&dur_time](float fr, float t) {
    float att_dur = 0.05f;
    float rel_dur = 0.05f;
    if (t < att_dur) {
      return t / att_dur;
    } else if (t > dur_time - rel_dur) {
      return (dur_time - t) / rel_dur;
    } else {
      return 1.0f;
    }
  };

  auto genSinTone = [&get_env, &dur_samples](float fr) {
  	std::vector< float > data(dur_samples, 0.0f);
  	for (uint32_t i = 0; i < data.size(); ++i) {
  		float t = i / float(Sound::SRATE);
  		//pure sine wave:
  		data[i] = std::sin(3.14159265358979f * 2.0f * fr * t);
  		//ramp up, ramp down:
      data[i] *= get_env(fr, t);
  	}
  	return new Sound::Sample(data);
  };

  auto genSawTone = [&get_env, &dur_samples](float fr) {
  	std::vector< float > data(dur_samples, 0.0f);
  	for (uint32_t i = 0; i < data.size(); ++i) {
  		float t = i / float(Sound::SRATE);
  		// sawtooth
  		data[i] = 2.0f * fr * fmod(t, 1.0f / fr) - 1.0f;
  		// ramp up, ramp down:
      data[i] *= get_env(fr, t);
  	}
  	return new Sound::Sample(data);
  };

  auto genSquTone = [&get_env, &dur_samples](float fr) {
  	std::vector< float > data(dur_samples, 0.0f);
  	for (uint32_t i = 0; i < data.size(); ++i) {
  		float t = i / float(Sound::SRATE);
  		// square wave:
  		data[i] = fr * fmod(t, 1.0f / fr) < 0.5 ? 0.0f : 1.0f;
  		// ramp up, ramp down:
      data[i] *= get_env(fr, t);
  	}
  	return new Sound::Sample(data);
  };

  std::mt19937 mt;
  mt.seed(0);

  std::array< std::function< Sound::Sample *(float) >, 3 > const gens = {
    genSinTone,
    genSawTone,
    genSquTone
  };

  // generate random sequence
  std::vector< uint32_t > indices(gens.size() * freqs.size(), 0);
  for (uint32_t i = 0; i < indices.size(); i++) indices[i] = i;
  std::shuffle(indices.begin(), indices.end(), mt);

  for (uint32_t i = 0; i < COUNT_PAIRS; i++) {
    // Pick based on the shuffle
    size_t seed = indices[i];
    size_t gen_i = seed / freqs.size();
    size_t freq_i = seed % freqs.size();
    ret->emplace_back(gens[gen_i](freqs[freq_i]));
  }

	return ret;

});

MatchMode::MatchMode() {

  std::mt19937 mt;
  mt.seed(0);
  //mt.seed(static_cast<unsigned int>(time(NULL)));

  std::vector< SDL_Keycode > pairings(*keys);
  std::shuffle(pairings.begin(), pairings.end(), mt);

  for (uint32_t i = 0; i < COUNT_PAIRS; i++) {
    cmap.emplace(std::make_pair(pairings[i * 2], (*sounds)[i]));
    cmap.emplace(std::make_pair(pairings[i * 2 + 1], (*sounds)[i]));
  }

}

MatchMode::~MatchMode() {

}

bool
MatchMode::handle_event(
  SDL_Event const &evt,
  glm::uvec2 const &window_size
) {

  if (evt.type == SDL_KEYDOWN) {

    // This gets rid of key repeats
    if (evt.key.repeat) return true;

    if (countdown > 0.0f) {
    // In memory phase, just play stuff
      if (!countdown_start) countdown_start = true;
      auto cmap_it = cmap.find(evt.key.keysym.sym);
      Sound::play(*(cmap_it->second), 1.0f, 0.0f);
      return true;
    }
    // In recall phase
    SDL_Keycode ks = evt.key.keysym.sym;
    std::cout << ks << std::endl;
    auto cmap_it = cmap.find(ks);
    if (cmap_it != cmap.end()) {
      if (!curr_held_active) { // This is the first key, held down
        curr_held_active = true;
        curr_held = evt.key.keysym.sym;
        std::cout << "1st " << ks << std::endl;
        Sound::play(*(cmap_it->second), 1.0f, -0.5f);
      } else { // This is the second key.
        if (ks == curr_held) return true;
        std::cout << "2nd " << ks << std::endl;
        Sound::play(*(cmap_it->second), 1.0f, 0.5f);

        // Check for a match
        if (cmap_it->second == cmap[curr_held]) {
          cmap.erase(curr_held);
          cmap.erase(ks);
          correct_count += 1;
          // TODO: play correct sound
        } else {
          wrong_count += 1;
          // TODO: play wrong sound
        }

      }
    }
    return true;
  } else if (evt.type == SDL_KEYUP) {
    if (curr_held_active) {
      if (evt.key.keysym.sym == curr_held) {
        curr_held_active = false;
      }
    }
    return true;
  }
  return false;
}

void MatchMode::update(float elapsed) {

  if (countdown_start && countdown > 0.0f) {
    countdown -= elapsed;
  }
  if (countdown < 0.0f && countdown_start) {
    countdown_start = false;
  }

}

void MatchMode::draw(glm::uvec2 const &drawable_size) {

}
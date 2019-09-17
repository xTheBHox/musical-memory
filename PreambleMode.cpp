#include "PreambleMode.hpp"

#include "Sprite.hpp"
#include "DrawSprites.hpp"
#include "Load.hpp"
#include "data_path.hpp"
#include "gl_errors.hpp"
#include "Sound.hpp"
#include "MatchMode.hpp"

Load< Sound::Sample const > snd_intro(
  LoadTagLate, []() -> Sound::Sample const * {

  return new Sound::Sample(data_path("sounds/intro.wav"));

});

PreambleMode::PreambleMode() {
  playing = Sound::play(*snd_intro);
}

PreambleMode::~PreambleMode() {

}

bool PreambleMode::handle_event(
  SDL_Event const &evt,
  glm::uvec2 const &window_size
) {
  if (evt.type == SDL_KEYDOWN) {
    if (evt.key.keysym.sym == SDLK_SPACE) {
      playing->stop();
    }
    return true;
  }
  
  return false;
}

void PreambleMode::update(float elapsed) {
  if (playing->stopped) {
    Mode::set_current(std::make_shared< MatchMode >());
  }
}

void PreambleMode::draw(glm::uvec2 const &drawable_size) {

}

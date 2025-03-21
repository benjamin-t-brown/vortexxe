#include "Ui.h"
#include "game/state.h"
#include "game/utils/StructGauge.hpp"
#include "lib/sdl2wrapper/Localization.h"
#include "lib/sdl2wrapper/Window.h"

namespace program {

namespace ui {
Ui::Ui(sdl2w::Window& window) : window(window) {}

void Ui::setup() {
  ProgressProps shieldBarProps = {
      .layout{.x = 300, .y = 5, .width = 50, .height = 12},
      .bgColor{255, 255, 255, 255},
      .fgColor{57, 120, 168, 255},
  };
  shieldBar.setProps(window, shieldBarProps);

  shieldText = sdl2w::L10n::trans(LOCSTR("Shield"));
  auto textSz = window.measureTextWithCurrentFont(shieldText);

  scoreTextLayout = {.x = 2, .y = 0};
  bonusTextLayout = {.x = 125, .y = 0};
  shieldBarTextLayout = {.x = shieldBarProps.layout.x - textSz.first - 5, .y = 0};

  livesTextLayout = {.x = 365, .y = 0};
  levelTextLayout = {.x = 440, .y = 0};

  menuTitleTextLayout = {.x = WINDOW_WIDTH / 2 - 22, .y = WINDOW_HEIGHT / 2 - 100};
  menuSubtitleTextLayout = {.x = WINDOW_WIDTH / 2 + 84 - 22, .y = WINDOW_HEIGHT / 2 - 88};
  menuLastScoreTextLayout = {.x = WINDOW_WIDTH / 2, .y = WINDOW_HEIGHT / 2 + 100};
  menuInstructionsShootTheseLayout = {.x = WINDOW_WIDTH / 2,
                                      .y = WINDOW_HEIGHT / 2 - 150 + 40};
  menuInstructionsDontShootTheseLayout = {.x = WINDOW_WIDTH / 2, .y = WINDOW_HEIGHT / 2 + 40};

  shootThese.reserve(3);
  shootThese.push_back(sdl2w::Animation());
  window.setAnimationFromDefinition("bonus0", shootThese.back());
  shootThese.push_back(sdl2w::Animation());
  window.setAnimationFromDefinition("star", shootThese.back());
  shootThese.push_back(sdl2w::Animation());
  window.setAnimationFromDefinition("shooting_star", shootThese.back());

  dontShootThese.reserve(3);
  dontShootThese.push_back(sdl2w::Animation());
  window.setAnimationFromDefinition("heart", dontShootThese.back());
  dontShootThese.push_back(sdl2w::Animation());
  window.setAnimationFromDefinition("candy", dontShootThese.back());
  dontShootThese.push_back(sdl2w::Animation());
  window.setAnimationFromDefinition("capsule", dontShootThese.back());

  bgRect = window.getStaticColorTexture(WINDOW_WIDTH, 22, {107, 37, 37, 255});
}

void Ui::drawLevelUi(const State& state) {

  window.drawTexture(bgRect, 0, 0, WINDOW_WIDTH, 22, false);

  window.setCurrentFont("default", 18);
  window.drawText(sdl2w::L10n::trans(LOCSTR("Score")) + std::string(" ") +
                      std::to_string(state.score),
                  scoreTextLayout.x,
                  scoreTextLayout.y,
                  {255, 255, 255});

  window.drawText(sdl2w::L10n::trans(LOCSTR("Bonus")) + std::string(" ") +
                      std::to_string(state.bonus),
                  bonusTextLayout.x,
                  bonusTextLayout.y,
                  {255, 255, 255});

  window.drawText(
      shieldText, shieldBarTextLayout.x, shieldBarTextLayout.y, {255, 255, 255});

  shieldBar.pct = 1. - structGaugeGetPct(state.ship.shieldGauge);
  shieldBar.draw(window);

  window.drawText(sdl2w::L10n::trans(LOCSTR("Lives")) + std::string(" ") +
                      std::to_string(state.lives),
                  livesTextLayout.x,
                  livesTextLayout.y,
                  {255, 255, 255});

  window.drawText(sdl2w::L10n::trans(LOCSTR("Level")) + std::string(" ") +
                      std::to_string(state.level),
                  levelTextLayout.x,
                  levelTextLayout.y,
                  {255, 255, 255});

  int gunInfoYOffset = 0;
  const int gunInfoY = 22;

  const std::string bigGunLabel = sdl2w::L10n::trans(LOCSTR("Big"));
  const std::string rapidGunLabel = sdl2w::L10n::trans(LOCSTR("Rapid"));
  const std::string spreadGunLabel = sdl2w::L10n::trans(LOCSTR("Spread"));

  if (state.ship.numBigShots > 0) {
    window.drawText(
        bigGunLabel + std::string(": ") + std::to_string(state.ship.numBigShots),
        2,
        gunInfoY + gunInfoYOffset,
        state.ship.selectedGun == SELECTED_GUN_LARGE ? SDL_Color{255, 0, 0}
                                                     : SDL_Color{255, 255, 255});
    gunInfoYOffset += 16;
  }
  if (state.ship.numRapidShots > 0) {
    window.drawText(
        rapidGunLabel + std::string(": ") + std::to_string(state.ship.numRapidShots),
        2,
        gunInfoY + gunInfoYOffset,
        state.ship.selectedGun == SELECTED_GUN_RAPID ? SDL_Color{255, 0, 0}
                                                     : SDL_Color{255, 255, 255});
    gunInfoYOffset += 16;
  }
  if (state.ship.numSpreadShots > 0) {
    window.drawText(
        spreadGunLabel + std::string(": ") + std::to_string(state.ship.numSpreadShots),
        2,
        gunInfoY + gunInfoYOffset,
        state.ship.selectedGun == SELECTED_GUN_SPREAD ? SDL_Color{255, 0, 0}
                                                      : SDL_Color{255, 255, 255});
    gunInfoYOffset += 16;
  }

  if (state.bonusMult > 1) {
    window.drawText("x" + std::to_string(state.bonusMult),
                    bonusTextLayout.x,
                    bonusTextLayout.y + 22,
                    {255, 255, 255});
  }

  // window.drawText("FPS: " + std::to_string(window.getAvgFps()),
  //                 2,
  //                 100,
  //                 {255, 255, 255});
}

void Ui::drawPostLevelUi(const State& state) {
  window.setCurrentFont("default", 18);
  window.drawTextCentered(sdl2w::L10n::trans(LOCSTR("Score")) + std::string(" ") +
                              std::to_string(state.score),
                          WINDOW_WIDTH / 2,
                          WINDOW_HEIGHT / 2,
                          {255, 255, 255});

  window.setCurrentFont("default", 18);
  window.drawTextCentered(
      sdl2w::L10n::trans(LOCSTR("Bonus")) + " " + std::to_string(state.bonus) +
          (state.bonusMult > 1 ? " x " + std::to_string(state.bonusMult) : ""),
      WINDOW_WIDTH / 2,
      WINDOW_HEIGHT / 2 - 32,
      state.bonus > 0 ? SDL_Color{95, 95, 255} : SDL_Color{255, 255, 255});
}

void Ui::drawGameOverUi() {
  window.drawTextCentered(sdl2w::L10n::trans(LOCSTR("Game Over")),
                          WINDOW_WIDTH / 2,
                          WINDOW_HEIGHT / 2,
                          {255, 0, 0});
}

void Ui::drawMenuUi(const State& state) {
  if (state.menuInd == 0) {
    window.setCurrentFont("default", 48);
    window.drawTextCentered(
        "VORTEX", menuTitleTextLayout.x - 2, menuTitleTextLayout.y - 2, {100, 100, 100});
    window.drawTextCentered(
        "VORTEX", menuTitleTextLayout.x, menuTitleTextLayout.y, {255, 255, 255});
    window.setCurrentFont("default", 36);
    window.drawTextCentered("XE",
                            menuSubtitleTextLayout.x - 2,
                            menuSubtitleTextLayout.y - 2,
                            {100, 100, 100});
    window.drawTextCentered(
        "XE", menuSubtitleTextLayout.x, menuSubtitleTextLayout.y, {255, 255, 0});

    window.setCurrentFont("default", 18);
    window.drawTextCentered(sdl2w::L10n::trans(LOCSTR("Last Score")) + " " +
                                std::to_string(state.lastScore),
                            menuLastScoreTextLayout.x,
                            menuLastScoreTextLayout.y,
                            {255, 255, 255});
    window.drawTextCentered(sdl2w::L10n::trans(LOCSTR("High Score")) + " " +
                                std::to_string(state.highScore),
                            menuLastScoreTextLayout.x,
                            menuLastScoreTextLayout.y + 36,
                            {255, 255, 255});
  } else if (state.menuInd == 1) {
    const int lineHeight = 64;
    const int startingX = WINDOW_WIDTH / 2 - lineHeight;

    window.setCurrentFont("default", 24);
    window.drawTextCentered(sdl2w::L10n::trans(LOCSTR("Shoot These")),
                            menuInstructionsShootTheseLayout.x,
                            menuInstructionsShootTheseLayout.y,
                            {255, 255, 255});

    for (int i = 0; i < static_cast<int>(shootThese.size()); i++) {
      int yOffset = 0;
      if (i == 2) {
        yOffset = -16;
      }
      window.drawAnimation(shootThese[i],
                           startingX + i * lineHeight,
                           menuInstructionsShootTheseLayout.y + lineHeight + yOffset);
    }

    window.drawTextCentered(sdl2w::L10n::trans(LOCSTR("Capture These (Don't Shoot)")),
                            menuInstructionsDontShootTheseLayout.x,
                            menuInstructionsDontShootTheseLayout.y,
                            {255, 255, 255});

    for (int i = 0; i < static_cast<int>(dontShootThese.size()); i++) {
      window.drawAnimation(dontShootThese[i],
                           startingX + i * lineHeight,
                           menuInstructionsDontShootTheseLayout.y + lineHeight);
    }
  }
}

} // namespace ui
} // namespace program
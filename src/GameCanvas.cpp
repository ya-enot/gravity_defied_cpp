#include "GameCanvas.h"

GameCanvas::GameCanvas(Micro* micro)
{
    splashImage = std::make_unique<Image>("splash.png");
    logoImage = std::make_unique<Image>("logo.png");

    // repaint();
    this->micro = micro;
    updateSizeAndRepaint();
    font = FontStorage::getFont(Font::STYLE_BOLD, Font::SIZE_MEDIUM);
    auto defaultFont = FontStorage::getFont(Font::STYLE_PLAIN, Font::SIZE_MEDIUM);

    helmetImage = std::make_unique<Image>("helmet.png");

    helmetSpriteWidth = helmetImage->getWidth() / 6;
    helmetSpriteHeight = helmetImage->getHeight() / 6;

    spritesImage = std::make_unique<Image>("sprites.png");

    dx = 0;
    dy = height2;

    commandMenu = new Command("Menu", Command::Type::SCREEN, 1);
    defaultFontWidth00 = defaultFont->stringWidth("00") + 3;
}

void GameCanvas::drawSprite(Graphics* g, int spriteNo, int x, int y)
{
    if (spritesImage) {
        g->setClip(x, y, spriteSizeX[spriteNo], spriteSizeY[spriteNo]);
        g->drawImage(
            spritesImage.get(),
            x - spriteOffsetX[spriteNo],
            y - spriteOffsetY[spriteNo],
            Graphics::TOP | Graphics::LEFT);
        g->setClip(0, 0, getWidth(), getHeight());
    }
}

void GameCanvas::requestRepaint(int var1)
{
    field_184 = var1;
    if (var1 == 0) {
        splashImage = nullptr;
        logoImage = nullptr;
    } else {
        repaint();
        serviceRepaints();
    }
}

void GameCanvas::method_124(bool var1)
{
    field_205 = var1;
    updateSizeAndRepaint();
}

void GameCanvas::updateSizeAndRepaint()
{
    width = getWidth();
    height = height2 = getHeight();

    repaint();
}

int GameCanvas::loadSprites(int flags)
{
    if (flags & 1) {
        if (!fenderImage) {
            fenderImage = std::make_unique<Image>("fender.png");
            fenderSpriteWidth = fenderImage->getWidth() / 6;
            fenderSpriteHeight = fenderImage->getHeight() / 6;
        }
        if (!engineImage) {
            engineImage = std::make_unique<Image>("engine.png");
            engineSpriteWidth = engineImage->getWidth() / 6;
            engineSpriteHeight = engineImage->getHeight() / 6;
        }
    } else {
        fenderImage = nullptr;
        engineImage = nullptr;
    }

    if (flags & 2) {
        if (!bodyPartsImages[1]) {
            bodyPartsImages[1] = std::make_unique<Image>("blueleg.png");
        }

        bodyPartsSpriteWidth[1] = bodyPartsImages[1]->getWidth() / 6;
        bodyPartsSpriteHeight[1] = bodyPartsImages[1]->getHeight() / 3;

        bodyPartsImages[0] = std::make_unique<Image>("bluearm.png");

        bodyPartsSpriteWidth[0] = bodyPartsImages[0]->getWidth() / 6;
        bodyPartsSpriteHeight[0] = bodyPartsImages[0]->getHeight() / 3;

        bodyPartsImages[2] = std::make_unique<Image>("bluebody.png");

        bodyPartsSpriteWidth[2] = bodyPartsImages[2]->getWidth() / 6;
        bodyPartsSpriteHeight[2] = bodyPartsImages[2]->getHeight() / 3;
    } else {
        bodyPartsImages[0] = nullptr;
        bodyPartsImages[1] = nullptr;
        bodyPartsImages[2] = nullptr;
    }

    return flags;
}

void GameCanvas::method_129()
{
    resetActions();
}

void GameCanvas::setViewPosition(int dx, int dy)
{
    this->dx = dx;
    this->dy = dy;
    gamePhysics->setRenderMinMaxX(-dx, -dx + width);
}

int GameCanvas::getDx()
{
    return dx;
}

int GameCanvas::addDx(int x)
{
    return x + dx;
}

int GameCanvas::addDy(int y)
{
    return -y + dy;
}

void GameCanvas::drawLine(int x, int y, int x2, int y2)
{
    graphics->drawLine(addDx(x), addDy(y), addDx(x2), addDy(y2));
}

void GameCanvas::drawLineF16(int x, int y, int x2, int y2)
{
    graphics->drawLine(addDx(x << 2 >> 16), addDy(y << 2 >> 16), addDx(x2 << 2 >> 16), addDy(y2 << 2 >> 16));
}

void GameCanvas::renderBodyPart(int x1F16, int y1F16, int x2F16, int y2F16, int bodyPartNo)
{
    renderBodyPart(x1F16, y1F16, x2F16, y2F16, bodyPartNo, 32768);
}

void GameCanvas::renderBodyPart(int x1F16, int y1F16, int x2F16, int y2F16, int bodyPartNo, int tF16)
{
    // t is the parameter of the linear interpolation
    // t == 0(0.0f)     => (x, y) == (x1, y1)
    // t == 65536(1.0f) => (x, y) == (x2, y2)
    // t == 32768(0.5f) => (x, y) == ((x1 + x2) / 2, (y1 + y2) / 2)
    int x = addDx(((int)((int64_t)x2F16 * (int64_t)tF16 >> 16) + (int)((int64_t)x1F16 * (int64_t)(65536 - tF16) >> 16)) >> 16);
    int y = addDy(((int)((int64_t)y2F16 * (int64_t)tF16 >> 16) + (int)((int64_t)y1F16 * (int64_t)(65536 - tF16) >> 16)) >> 16);

    int angleFP16 = MathF16::atan2F16(x2F16 - x1F16, y2F16 - y1F16);
    int spriteNo = calcSpriteNo(angleFP16, 0, 205887, 16, false);

    if (bodyPartsImages[bodyPartNo]) {
        x -= bodyPartsSpriteWidth[bodyPartNo] / 2;
        y -= bodyPartsSpriteHeight[bodyPartNo] / 2;
        graphics->setClip(x, y, bodyPartsSpriteWidth[bodyPartNo], bodyPartsSpriteHeight[bodyPartNo]);
        graphics->drawImage(
            bodyPartsImages[bodyPartNo].get(),
            x - bodyPartsSpriteWidth[bodyPartNo] * (spriteNo % 6),
            y - bodyPartsSpriteHeight[bodyPartNo] * (spriteNo / 6),
            Graphics::TOP | Graphics::LEFT);
        graphics->setClip(0, 0, width, getHeight());
    }
}

void GameCanvas::method_142(int var1, int var2, int var3, int var4)
{
    ++var3;
    int var5 = addDx(var1 - var3);
    int var6 = addDy(var2 + var3);
    int var7 = var3 << 1;
    if ((var4 = -((int)(((int64_t)((int)((int64_t)var4 * 11796480L >> 16)) << 32) / 205887L >> 16))) < 0) {
        var4 += 360;
    }

    graphics->drawArc(var5, var6, var7, var7, (var4 >> 16) + 170, 90);
}

void GameCanvas::drawCircle(int x, int y, int size)
{
    int radius = size / 2;
    int localX = addDx(x - radius);
    int localY = addDy(y + radius);
    graphics->drawArc(localX, localY, size, size, 0, 360);
}

void GameCanvas::fillRect(int x, int y, int w, int h)
{
    int var5 = addDx(x);
    int var6 = addDy(y);
    graphics->fillRect(var5, var6, w, h);
}

void GameCanvas::drawForthSpriteByCenter(int centerX, int centerY)
{
    int halfSizeX = spriteSizeX[4] / 2;
    int halfSizeY = spriteSizeY[4] / 2;
    drawSprite(graphics, 4, addDx(centerX - halfSizeX), addDy(centerY + halfSizeY));
}

void GameCanvas::drawHelmet(int x, int y, int angleF16)
{
    int var4 = calcSpriteNo(angleF16, -102943, 411774, 32, true);
    if (helmetImage != nullptr) {
        int var5 = addDx(x) - helmetSpriteWidth / 2;
        int var6 = addDy(y) - helmetSpriteHeight / 2;
        graphics->setClip(var5, var6, helmetSpriteWidth, helmetSpriteHeight);
        graphics->drawImage(
            helmetImage.get(),
            var5 - helmetSpriteWidth * (var4 % 6),
            var6 - helmetSpriteHeight * (var4 / 6),
            Graphics::TOP | Graphics::LEFT);
        graphics->setClip(0, 0, width, getHeight());
    }
}

void GameCanvas::drawTime(uint64_t time10Ms)
{
    const std::string timeStr = Time::timeToString(time10Ms);
    setColor(0, 0, 0);
    graphics->drawString(
        timeStr,
        width - defaultFontWidth00,
        height2 - GlobalSetting::TimerFpsTextOffset,
        Graphics::BOTTOM | Graphics::RIGHT);
}

void GameCanvas::method_150(int var1)
{
    if (timerId == var1) {
        timerTriggered = true;
    }
}

void GameCanvas::method_151()
{
    field_226 += 655;
    int var0 = 32768 + ((MathF16::sinF16(field_226) < 0 ? -MathF16::sinF16(field_226) : MathF16::sinF16(field_226)) >> 1);
    flagAnimationTime += (int)(6553L * (int64_t)var0 >> 16);
}

void GameCanvas::renderStartFlag(int x, int y)
{
    if (flagAnimationTime > 229376) {
        flagAnimationTime = 0;
    }

    setColor(0, 0, 0);
    drawLine(x, y, x, y + 32);
    drawSprite(graphics, startFlagAnimationTimeToSpriteNo[flagAnimationTime >> 16], addDx(x), addDy(y) - 32);
}

void GameCanvas::renderFinishFlag(int x, int y)
{
    if (flagAnimationTime > 229376) {
        flagAnimationTime = 0;
    }

    setColor(0, 0, 0);
    drawLine(x, y, x, y + 32);
    drawSprite(graphics, finishFlagAnumationTimeToSpriteNo[flagAnimationTime >> 16], addDx(x), addDy(y) - 32);
}

void GameCanvas::drawWheelTires(int x, int y, int wheelIsThin)
{
    int spriteNo;
    if (wheelIsThin == 1) {
        spriteNo = 0;
    } else {
        spriteNo = 1;
    }

    int spriteHalfX = spriteSizeX[spriteNo] / 2;
    int spriteHalfY = spriteSizeY[spriteNo] / 2;
    int centerX = addDx(x - spriteHalfX);
    int centerY = addDy(y + spriteHalfY);
    drawSprite(graphics, spriteNo, centerX, centerY);
}

int GameCanvas::calcSpriteNo(int angleF16, int var2, int var3, int var4, bool var5)
{
    for (angleF16 += var2; angleF16 < 0; angleF16 += var3) {
    }

    while (angleF16 >= var3) {
        angleF16 -= var3;
    }

    if (var5) {
        angleF16 = var3 - angleF16;
    }

    int var6;
    return (var6 = (int)((int64_t)((int)(((int64_t)angleF16 << 32) / (int64_t)var3 >> 16)) * (int64_t)(var4 << 16) >> 16)) >> 16 < var4 - 1 ? var6 >> 16 : var4 - 1;
}

void GameCanvas::renderEngine(int x, int y, int angleF16)
{
    int spriteNo = calcSpriteNo(angleF16, -247063, 411774, 32, true);
    int centerX = addDx(x) - engineSpriteWidth / 2;
    int centerY = addDy(y) - engineSpriteHeight / 2;
    if (engineImage != nullptr) {
        graphics->setClip(centerX, centerY, engineSpriteWidth, engineSpriteHeight);
        graphics->drawImage(
            engineImage.get(),
            centerX - engineSpriteWidth * (spriteNo % 6),
            centerY - engineSpriteHeight * (spriteNo / 6),
            Graphics::TOP | Graphics::LEFT);
        graphics->setClip(0, 0, width, getHeight());
    }
}

void GameCanvas::renderFender(int x, int y, int angleF16)
{
    int spriteNo = calcSpriteNo(angleF16, -185297, 411774, 32, true);
    if (fenderImage != nullptr) {
        int centerX = addDx(x) - fenderSpriteWidth / 2;
        int centerY = addDy(y) - fenderSpriteHeight / 2;
        graphics->setClip(centerX, centerY, fenderSpriteWidth, fenderSpriteHeight);
        graphics->drawImage(
            fenderImage.get(),
            centerX - fenderSpriteWidth * (spriteNo % 6),
            centerY - fenderSpriteHeight * (spriteNo / 6),
            Graphics::TOP | Graphics::LEFT);
        graphics->setClip(0, 0, width, getHeight());
    }
}

void GameCanvas::clearScreenWithWhite()
{
    graphics->setColor(255, 255, 255);
    graphics->fillRect(0, 0, width, height2);
}

void GameCanvas::setColor(int red, int green, int blue)
{
    if (Micro::isInGameMenu) {
        red += 128;
        green += 128;
        blue += 128;
        if (red > 240) {
            red = 240;
        }

        if (green > 240) {
            green = 240;
        }

        if (blue > 240) {
            blue = 240;
        }
    }

    graphics->setColor(red, green, blue);
}

void GameCanvas::drawGame(Graphics* g)
{
    // synchronized (objectForSyncronization) {
    if (micro->gameStarted && !micro->gameDestroyed) {
        graphics = g;

        int var3;
        if (field_184 != 0) {
            if (field_184 == 1) {
                graphics->setColor(255, 255, 255);
                graphics->fillRect(0, 0, getWidth(), getHeight());
                if (logoImage != nullptr) {
                    graphics->drawImage(
                        logoImage.get(),
                        getWidth() / 2,
                        getHeight() / 2,
                        logoImage->getWidth() * GlobalSetting::LogoMultiplier,
                        logoImage->getHeight() * GlobalSetting::LogoMultiplier,
                        Graphics::HCENTER | Graphics::VCENTER);
                    drawSprite(graphics, 16, getWidth() - spriteSizeX[16] - 5 - GlobalSetting::BarScreenOffset, getHeight() - spriteSizeY[16] - 7 - GlobalSetting::BarH);
                    drawSprite(graphics, 17, getWidth() - spriteSizeX[17] - 4 - GlobalSetting::BarScreenOffset, getHeight() - spriteSizeY[17] - spriteSizeY[16] - 9 - GlobalSetting::BarH);
                }
            } else {
                graphics->setColor(255, 255, 255);
                graphics->fillRect(0, 0, getWidth(), getHeight());

                if (splashImage != nullptr) {
                    graphics->drawImage(
                        splashImage.get(),
                        getWidth() / 2,
                        getHeight() / 2,
                        splashImage->getWidth() * GlobalSetting::SplashMultiplier,
                        splashImage->getHeight() * GlobalSetting::SplashMultiplier,
                        Graphics::HCENTER | Graphics::VCENTER);
                }
            }

            var3 = (((int64_t)Micro::gameLoadingStateStage << 48) / (11L << 16)) >> 16;
            drawProgressBar(var3, true);
        } else {
            if (height != getHeight()) {
                updateSizeAndRepaint();
            }

            gamePhysics->setMotoComponents();
            setViewPosition(-gamePhysics->getCamPosX() + field_178 + width / 2, gamePhysics->getCamPosY() + field_179 + height2 / 2);
            gamePhysics->renderGame(this);
            if (isDrawingTime) {
                drawTime(micro->gameTimeMs / 10L);
            }

            if (!timerMessage.empty()) {
                setColor(0, 0, 0);
                graphics->setFont(font);
                if (height2 <= 128) {
                    graphics->drawString(
                        timerMessage,
                        width / 2,
                        1,
                        Graphics::TOP | Graphics::HCENTER);
                } else {
                    graphics->drawString(
                        timerMessage,
                        width / 2,
                        height2 / 4,
                        Graphics::BOTTOM | Graphics::HCENTER);
                }

                if (timerTriggered) {
                    timerTriggered = false;
                    timerMessage = "";
                }
            }

            if (GlobalSetting::ShowFPS) {
                setColor(0, 0, 0);
                graphics->setFont(font);
                graphics->drawString(
                    "FPS: " + std::to_string(fps),
                    defaultFontWidth00,
                    height2 - GlobalSetting::TimerFpsTextOffset,
                    Graphics::BOTTOM | Graphics::LEFT);
            }

            var3 = gamePhysics->method_52();
            drawProgressBar(var3, false);
        }

        graphics = nullptr;
    }
    // }
}

// draw progressbar
void GameCanvas::drawProgressBar(int var1, bool mode)
{
    const int h = mode ? height : height2;
    const int barX = 1;
    const int barY = h - GlobalSetting::BarScreenOffset - GlobalSetting::BarH;

    var1 *= (var1 >= 0);

    setColor(0, 0, 0);
    graphics->fillRect(barX, barY, width - (2 * barX), GlobalSetting::BarH);

    setColor(255, 255, 255);
    const int loadingBarX = barX + GlobalSetting::LoadingBarPadding;
    graphics->fillRect(
        loadingBarX,
        barY + GlobalSetting::LoadingBarPadding,
        (int)((int64_t)((width - 2 * loadingBarX) << 16) * (int64_t)var1 >> 16) >> 16,
        GlobalSetting::BarH - (2 * GlobalSetting::LoadingBarPadding));
}

// void GameCanvas::method_163(int var1)
// {
//     field_232 = var1;
// }

void GameCanvas::paint(Graphics* graphics)
{
    static int64_t time = 0;

    if (GlobalSetting::ShowFPS) {
        const int64_t now = Time::currentTimeMillis();
        const int64_t delta = now - time;
        time = now;

        if (delta != 0) {
            fps = 1000 / delta;
            // setWindowTitle(std::string("Gravity Defied. FPS: ") + std::to_string(fps));
        }
    }

    processTimers(); // We need to call this function as often as we can. It might be better to move this call somewhere.
    if (Micro::isInGameMenu && menuManager != nullptr) {
        menuManager->method_202(graphics);
    } else {
        drawGame(graphics);
    }
}

void GameCanvas::resetActions()
{
    activeActions[Keys::UP] = false;
    activeActions[Keys::DOWN] = false;
    activeActions[Keys::LEFT] = false;
    activeActions[Keys::RIGHT] = false;
    activeActions[Keys::FIRE] = false;
    activeActions[Keys::BACK] = false;
}

void GameCanvas::handleUpdatedInput()
{
    int verticalMovement = 0;
    int horizontalMovement = 0;

    for (const auto& [action, isActive] : activeActions) {
        if (!isActive) {
            continue;
        }

        verticalMovement += availableActions[action][0];
        horizontalMovement += availableActions[action][1];
    }

    gamePhysics->method_30(verticalMovement, horizontalMovement);
}

void GameCanvas::processTimers()
{
    for (auto i = timers.begin(); i != timers.end();) {
        if (i->ready()) {
            method_150(i->getId());
            i = timers.erase(i);
        } else {
            i++;
        }
    }
}

void GameCanvas::processKeyPressed(const Keys keyCode)
{
    activeActions[keyCode] = true;
    handleUpdatedInput();
}

void GameCanvas::processKeyReleased(const Keys keyCode)
{
    activeActions[keyCode] = false;
    handleUpdatedInput();
}

void GameCanvas::init(GamePhysics* gamePhysics)
{
    this->gamePhysics = gamePhysics;
    gamePhysics->setMinimalScreenWH(width < height2 ? width : height2);
}

void GameCanvas::scheduleGameTimerTask(std::string timerMessage, int delayMs)
{
    timerTriggered = false;
    ++timerId;
    this->timerMessage = timerMessage;
    timers.push_back(Timer(timerId, delayMs));
}

void GameCanvas::setMenuManager(MenuManager* menuManager)
{
    this->menuManager = menuManager;
}

void GameCanvas::method_168(Command* var1, Displayable* var2)
{
    (void)var2;
    if (var1 == commandMenu) {
        menuManager->field_377 = true;
        micro->gameToMenu();
    }
}

void GameCanvas::keyPressed(const Keys var1)
{
    if (Micro::isInGameMenu && menuManager != nullptr) {
        menuManager->processKeyCode(var1);
        return;
    }

    processKeyPressed(var1);
}

void GameCanvas::keyReleased(const Keys var1)
{
    if (var1 == Keys::BACK) {
        pressedEsc();
    }

    processKeyReleased(var1);
}

void GameCanvas::commandAction(Command* var1, Displayable* var2)
{
    if (Micro::isInGameMenu && menuManager != nullptr) {
        menuManager->method_206(var1, var2);
    } else {
        method_168(var1, var2);
    }
}

void GameCanvas::removeMenuCommand()
{
    removeCommand(commandMenu);
}

void GameCanvas::addMenuCommand()
{
    addCommand(commandMenu);
}
#include "LevelLoader.h"

int LevelLoader::field_133 = 0;
int LevelLoader::field_134 = 0;
int LevelLoader::field_135 = 0;
int LevelLoader::field_136 = 0;

const int LevelLoader::field_114 = 0;
const int LevelLoader::field_115 = 1;
const int LevelLoader::field_116 = 2;
const int LevelLoader::field_117 = 0;
const int LevelLoader::field_118 = 1;
bool LevelLoader::isEnabledPerspective = true;
bool LevelLoader::isEnabledShadows = true;

LevelLoader::LevelLoader(const std::filesystem::path& path)
{
    for (int i = 0; i < 3; ++i) {
        field_123[i] = (int)((int64_t)((GamePhysics::const175_1_half[i] + 19660) >> 1) * (int64_t)((GamePhysics::const175_1_half[i] + 19660) >> 1) >> 16);
        field_124[i] = (int)((int64_t)((GamePhysics::const175_1_half[i] - 19660) >> 1) * (int64_t)((GamePhysics::const175_1_half[i] - 19660) >> 1) >> 16);
    }

    this->mrgFilePath = path;
    trackHeaders = MRGLoader::loadLevels(this->mrgFilePath);
    loadNextTrack();
}

std::string LevelLoader::getName(const int level, const int track) const
{
    const MRGLoader::LevelTracks l = trackHeaders.at(level);
    const int tracksCount = static_cast<int>(l.tracks.size());

    if (level < 3 && track < tracksCount) {
        return l.tracks.at(track).trackName;
    }

    return "---";
}

int LevelLoader::getTracksCount(const int level) const 
{
    const MRGLoader::LevelTracks l = trackHeaders.at(level);
    return static_cast<int>(l.tracks.size());
}

std::vector<std::string> LevelLoader::GetTrackNames(const int level) const 
{
    const MRGLoader::LevelTracks levelTracks = trackHeaders.at(level);
    std::vector<std::string> trackNames{};

    for (auto &track: levelTracks.tracks) {
        trackNames.push_back(track.trackName);
    }

    return trackNames;
}

void LevelLoader::loadNextTrack()
{
    loadTrack(loadedLevel, loadedTrack + 1);
}

int LevelLoader::loadTrack(const int level, const int track)
{
    Log::write(Log::LogLevel::Debug, "loadTrack %d %d\n", level, track);
    loadedLevel = level;
    loadedTrack = track;

    const MRGLoader::LevelTracks l = trackHeaders.at(level);
    const int tracksCount = static_cast<int>(l.tracks.size());

    if (loadedTrack >= tracksCount) {
        loadedTrack = 0;
    }

    const MRGLoader::Track t = l.tracks.at(loadedTrack);
    FileStream levelFileStream{mrgFilePath, std::ios::in | std::ios::binary};
    levelFileStream.setPos(t.offset);

    if (gameLevel == nullptr) {
        gameLevel = new GameLevel();
    }

    gameLevel->load(&levelFileStream);
    method_96(gameLevel);
    return loadedTrack;
}

void LevelLoader::method_90(int var1)
{
    (void)var1;
    field_129 = gameLevel->startPosX << 1;
    field_130 = gameLevel->startPosY << 1;
}

int LevelLoader::method_91()
{
    return gameLevel->pointPositions[gameLevel->finishFlagPoint][0] << 1;
}

int LevelLoader::method_92()
{
    return gameLevel->pointPositions[gameLevel->startFlagPoint][0] << 1;
}

int LevelLoader::method_93()
{
    return gameLevel->startPosX << 1;
}

int LevelLoader::method_94()
{
    return gameLevel->startPosY << 1;
}

int LevelLoader::method_95(int var1)
{
    return gameLevel->method_181(var1 >> 1);
}

void LevelLoader::method_96(GameLevel* gameLevel)
{
    field_131 = INT_MIN;
    this->gameLevel = gameLevel;
    int var2 = gameLevel->pointsCount;

    if (field_121.empty() || field_132 < var2) {
        field_132 = var2 < 100 ? 100 : var2;
        field_121.assign(field_132, std::vector<int>(2));
    }

    field_133 = 0;
    field_134 = 0;
    field_135 = gameLevel->pointPositions[field_133][0];
    field_136 = gameLevel->pointPositions[field_134][0];

    for (int var3 = 0; var3 < var2; ++var3) {
        int var4 = gameLevel->pointPositions[(var3 + 1) % var2][0] - gameLevel->pointPositions[var3][0];
        int var5 = gameLevel->pointPositions[(var3 + 1) % var2][1] - gameLevel->pointPositions[var3][1];

        if (var3 != 0 && var3 != var2 - 1) {
            field_131 = field_131 < gameLevel->pointPositions[var3][0] ? gameLevel->pointPositions[var3][0] : field_131;
        }

        int var6 = -var5;
        int var8 = GamePhysics::getSmthLikeMaxAbs(var6, var4);
        field_121[var3][0] = (int)(((int64_t)var6 << 32) / (int64_t)var8 >> 16);
        field_121[var3][1] = (int)(((int64_t)var4 << 32) / (int64_t)var8 >> 16);

        if (gameLevel->startFlagPoint == 0 && gameLevel->pointPositions[var3][0] > gameLevel->startPosX) {
            gameLevel->startFlagPoint = var3 + 1;
        }

        if (gameLevel->finishFlagPoint == 0 && gameLevel->pointPositions[var3][0] > gameLevel->finishPosX) {
            gameLevel->finishFlagPoint = var3;
        }
    }

    field_133 = 0;
    field_134 = 0;
    field_135 = 0;
    field_136 = 0;
}

void LevelLoader::setMinMaxX(int minX, int maxX)
{
    gameLevel->setMinMaxX(minX, maxX);
}

void LevelLoader::renderLevel3D(GameCanvas* gameCanvas, int xF16, int yF16)
{
    if (gameCanvas != nullptr) {
        gameCanvas->setColor(0, 170, 0);
        xF16 >>= 1;
        yF16 >>= 1;
        gameLevel->renderLevel3D(gameCanvas, xF16, yF16);
    }
}

void LevelLoader::renderTrackNearestLine(GameCanvas* canvas)
{
    canvas->setColor(0, 255, 0);
    gameLevel->renderTrackNearestGreenLine(canvas);
}

void LevelLoader::method_100(int var1, int var2, int var3)
{
    gameLevel->method_184((var1 + 98304) >> 1, (var2 - 98304) >> 1, var3 >> 1);
    var2 >>= 1;
    var1 >>= 1;
    field_134 = field_134 < gameLevel->pointsCount - 1 ? field_134 : gameLevel->pointsCount - 1;
    field_133 = field_133 < 0 ? 0 : field_133;

    if (var2 > field_136) {
        while (field_134 < gameLevel->pointsCount - 1 && var2 > gameLevel->pointPositions[++field_134][0]) {
        }
    } else if (var1 < field_135) {
        while (field_133 > 0 && var1 < gameLevel->pointPositions[--field_133][0]) {
        }
    } else {
        while (field_133 < gameLevel->pointsCount && var1 > gameLevel->pointPositions[++field_133][0]) {
        }

        if (field_133 > 0) {
            --field_133;
        }

        while (field_134 > 0 && var2 < gameLevel->pointPositions[--field_134][0]) {
        }

        field_134 = field_134 + 1 < gameLevel->pointsCount - 1 ? field_134 + 1 : gameLevel->pointsCount - 1;
    }

    field_135 = gameLevel->pointPositions[field_133][0];
    field_136 = gameLevel->pointPositions[field_134][0];
}

int LevelLoader::method_101(TimerOrMotoPartOrMenuElem* var1, int var2)
{
    int var16 = 0;
    int8_t var17 = 2;
    int var18 = var1->xF16 >> 1;
    int var19 = var1->yF16 >> 1;

    if (isEnabledPerspective) {
        var19 -= 65536;
    }

    int var20 = 0, var21 = 0;

    for (int var22 = field_133; var22 < field_134; ++var22) {
        int var4 = gameLevel->pointPositions[var22][0];
        int var5 = gameLevel->pointPositions[var22][1];
        int var6 = gameLevel->pointPositions[var22 + 1][0];
        int var7;

        if ((var7 = gameLevel->pointPositions[var22 + 1][1]) < var5) {
            ;
        }

        if (var18 - field_123[var2] <= var6 && var18 + field_123[var2] >= var4) {
            int var8 = var4 - var6;
            int var9 = var5 - var7;
            int var10 = (int)((int64_t)var8 * (int64_t)var8 >> 16) + (int)((int64_t)var9 * (int64_t)var9 >> 16);
            int var11 = (int)((int64_t)(var18 - var4) * (int64_t)(-var8) >> 16) + (int)((int64_t)(var19 - var5) * (int64_t)(-var9) >> 16);
            int var12;

            if ((var10 < 0 ? -var10 : var10) >= 3) {
                var12 = (int)(((int64_t)var11 << 32) / (int64_t)var10 >> 16);
            } else {
                var12 = (var11 > 0 ? 1 : -1) * (var10 > 0 ? 1 : -1) * INT_MAX;
            }

            if (var12 < 0) {
                var12 = 0;
            }

            if (var12 > 65536) {
                var12 = 65536;
            }

            int var13 = var4 + (int)((int64_t)var12 * (int64_t)(-var8) >> 16);
            int var14 = var5 + (int)((int64_t)var12 * (int64_t)(-var9) >> 16);
            var8 = var18 - var13;
            var9 = var19 - var14;
            int8_t var3;
            int64_t var23;

            if ((var23 = ((int64_t)var8 * (int64_t)var8 >> 16) + ((int64_t)var9 * (int64_t)var9 >> 16)) < (int64_t)field_123[var2]) {
                if (var23 >= (int64_t)field_124[var2]) {
                    var3 = 1;
                } else {
                    var3 = 0;
                }
            } else {
                var3 = 2;
            }

            if (var3 == 0 && (int)((int64_t)field_121[var22][0] * (int64_t)var1->field_382 >> 16) + (int)((int64_t)field_121[var22][1] * (int64_t)var1->field_383 >> 16) < 0) {
                field_137 = field_121[var22][0];
                field_138 = field_121[var22][1];
                return 0;
            }

            if (var3 == 1 && (int)((int64_t)field_121[var22][0] * (int64_t)var1->field_382 >> 16) + (int)((int64_t)field_121[var22][1] * (int64_t)var1->field_383 >> 16) < 0) {
                ++var16;
                var17 = 1;

                if (var16 == 1) {
                    var20 = field_121[var22][0];
                    var21 = field_121[var22][1];
                } else {
                    var20 += field_121[var22][0];
                    var21 += field_121[var22][1];
                }
            }
        }
    }

    if (var17 == 1) {
        if ((int)((int64_t)var20 * (int64_t)var1->field_382 >> 16) + (int)((int64_t)var21 * (int64_t)var1->field_383 >> 16) >= 0) {
            return 2;
        }

        field_137 = var20;
        field_138 = var21;
    }

    return var17;
}

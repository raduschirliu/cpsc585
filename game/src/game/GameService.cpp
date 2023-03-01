#include "game/GameService.h"

#include <chrono>

#include "engine/input/InputService.h"
#include "engine/scene/Entity.h"
#include "engine/service/ServiceProvider.h"

GameData game_data;
bool has_finished;
bool game_is_running;

std::string_view GameService::GetName() const
{
    return "GameService";
}

void GameService::OnInit()
{
}

void GameService::OnStart(ServiceProvider& service_provider)
{
    game_is_running = false;

    game_data.number_of_players = 4;
    game_data.players_finished = 0;

    game_data.time_elapsed = 0.0f;
    game_data.fastest_lap = 0.0f;

    game_data.num_laps = 3;

    game_data.player_1_laps_finished = 0;
    game_data.player_2_laps_finished = 0;
    game_data.player_3_laps_finished = 0;
    game_data.player_4_laps_finished = 0;

    game_data.player_1_kills = 0;
    game_data.player_2_kills = 0;
    game_data.player_3_kills = 0;
    game_data.player_4_kills = 0;

    game_data.player_1_deaths = 0;
    game_data.player_2_deaths = 0;
    game_data.player_3_deaths = 0;
    game_data.player_4_deaths = 0;

    Countdown();
}

void GameService::OnUpdate()
{
    UpdateTime();

    /*
    This stuff will be done when we figure out kills, deaths, etc.
    Just left some pseudocode for now until we figure it out

    if(player1 touches finish line){
        game_data.player_1_laps_finished++;
    }
    if(player2 touches finish line){
        game_data.player_2_laps_finished++;
    }
    if(player3 touches finish line){
        game_data.player_3_laps_finished++;
    }
    if(player4 touches finish line){
        game_data.player_4_laps_finished++;
    }
    if(player1 kills player2){
        game_data.player_1_kills++;
        game_data.player_2_deaths++;
    }
    if(player1 kills player3){
        game_data.player_1_kills++;
        game_data.player_3_deaths++;
    }
    if(player1 kills player4){
        game_data.player_1_kills++;
        game_data.player_4_deaths++;
    }
    if(player2 kills player1){
        game_data.player_2_kills++;
        game_data.player_1_deaths++;
    }
    if(player2 kills player3){
        game_data.player_2_kills++;
        game_data.player_3_deaths++;
    }
    if(player2 kills player4){
        game_data.player_2_kills++;
        game_data.player_4_deaths++;
    }
    if(player3 kills player1){
        game_data.player_3_kills++;
        game_data.player_1_deaths++;
    }
    if(player3 kills player2){
        game_data.player_3_kills++;
        game_data.player_2_deaths++;
    }
    if(player3 kills player4){
        game_data.player_3_kills++;
        game_data.player_4_deaths++;
    }
    if(player4 kills player1){
        game_data.player_4_kills++;
        game_data.player_1_deaths++;
    }
    if(player4 kills player2){
        game_data.player_4_kills++;
        game_data.player_2_deaths++;
    }
    if(player4 kills player3){
        game_data.player_4_kills++;
        game_data.player_3_deaths++;
    }
    */
    if (game_data.player_1_laps_finished == game_data.num_laps)
    {
        game_data.players_finished++;
        game_data.player_1_laps_finished = 0;
        // do this so that it doesn't continiously increment. since we
        // are in update we have incremented players finished already so
        // we don't care about laps anymore... right???
    }
    if (game_data.player_2_laps_finished == game_data.num_laps)
    {
        game_data.players_finished++;
        game_data.player_2_laps_finished = 0;
    }
    if (game_data.player_3_laps_finished == game_data.num_laps)
    {
        game_data.players_finished++;
        game_data.player_3_laps_finished = 0;
    }
    if (game_data.player_4_laps_finished == game_data.num_laps)
    {
        game_data.players_finished++;
        game_data.player_4_laps_finished = 0;
    }
    if (game_data.players_finished == 4)
    {
        GameOver();
    }
}

void GameService::OnCleanup()
{
}

double GameService::UpdateTime()
{
    std::chrono::time_point<std::chrono::system_clock> endTime;
    std::chrono::time_point<std::chrono::system_clock> m_StartTime;
    std::chrono::time_point<std::chrono::system_clock> m_EndTime;

    if (game_is_running)
    {
        endTime = std::chrono::system_clock::now();
    }
    else
    {
        endTime = m_EndTime;
    }

    return std::chrono::duration_cast<std::chrono::milliseconds>(endTime -
                                                                 m_StartTime)
        .count();
}

void GameService::Countdown()
{
    _sleep(3);
    game_is_running = true;
}

void GameService::GameOver()
{
    // todo
}
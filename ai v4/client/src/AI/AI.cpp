#include <stdlib.h>
#include "AI.h"

#include <vector>

using namespace std;

typedef pair<int, int> pii;
typedef vector<pair<int,Cell> > ArayOfTarget;
////////////////////////////////////////////////////
static vector <pair<int , Cell> > vec;


////////////////// ehsan2
static int damage_taken[10] ;
static int damage_deal[4];

pair<int,Cell> findBestPosWithRange(Hero* my_hero,vector<Hero*> opposition, World* world,Ability abil,int& maxManInArea){
    Map map = world->getMap();
    int n = map.getRowNum();
    int m = map.getColumnNum();
    pair<int,Cell> targetCellForBomb ;
    int areaCanBomb = abil.getAreaOfEffect();
    for(int i = 0 ;i < n ;i++){
        for(int j = 0 ;j < m;j++){
            if(world->manhattanDistance(map.getCell(i,j), my_hero->getCurrentCell()) <= abil.getRange()){
            int tempNumHero = 0;
            for (Hero *opp_hero : opposition){

                if (opp_hero->getCurrentCell().isInVision())//if hero is seen
                    if (areaCanBomb >= world->manhattanDistance(opp_hero->getCurrentCell(),map.getCell(i,j)))
                        tempNumHero++;

            }
                if(tempNumHero >= maxManInArea){
                    targetCellForBomb.second = map.getCell(i,j);
                    maxManInArea = tempNumHero ;
                }
            }
        }
    }
    return targetCellForBomb ;
}

ArayOfTarget findForAllHeroBestPosWithRange(vector<Hero*> my_heroes, vector<Hero*> opposition,
        World* world,vector<Ability> abil){
    ArayOfTarget arayOfTraget;
    int helpDamageDeal[4],alaki=0;
    memset(helpDamageDeal,0,sizeof(helpDamageDeal));
    for(int i = 0 ;i < my_heroes.size();i++){
        Hero* hero = my_heroes[i];
        Ability hero_ability = abil[i];
        arayOfTraget.push_back(findBestPosWithRange(hero,opposition,world,hero_ability,alaki));
        for(int j = 0 ;j < 4;j++)
            helpDamageDeal[j]+=damage_deal[j];
    }
    for(int i = 0;i < 4;i++)
        damage_deal[i] = helpDamageDeal[i];
    return arayOfTraget;
}

pair<int,Cell> calcBLASTER(Hero* my_hero, int& ap, World* world,int index)
{
    int maxManInArea = 0;
    Ability abil = my_hero->getAbility(AbilityName::BLASTER_BOMB);
    Map map = world->getMap();
    if(abil.isReady()){
        pair<int,Cell> targetCellForBomb = findBestPosWithRange(my_hero,world->getOppHeroes(), world, abil,maxManInArea);
        targetCellForBomb.first = 0 ;
        if(maxManInArea > 0){
            cout<<"$$$$$$$$$$$$$$$$$$$$ BOMB "<<endl;
            damage_deal[index] = maxManInArea * abil.getPower();
            ap += abil.getAPCost() ;
            return targetCellForBomb;
        }
    }
    maxManInArea = 0;
    abil = my_hero->getAbility(AbilityName::BLASTER_ATTACK);
    pair<int,Cell> targetCellForBomb = findBestPosWithRange(my_hero,world->getOppHeroes(), world, abil, maxManInArea);
    targetCellForBomb.first = 1 ;
    if(maxManInArea > 0 ){
        cout<<"$$$$$$$$$$$$$$$$$$$$ atack "<<endl;
        ap += abil.getAPCost() ;
        damage_deal[index] = maxManInArea * abil.getPower();
        return targetCellForBomb;
    }

    targetCellForBomb.first = -1 ;
    targetCellForBomb.second = Cell::NULL_CELL ;
    damage_deal[index] = maxManInArea ;
    return targetCellForBomb;
}


void calcDamageTaken(World* world)
{
  int min_dist = 1000000 ;
  for(int i=0;i<6;i++)
    damage_taken[i] = 0 ;
  for (Hero *opp_hero : world->getOppHeroes()) {
      if (opp_hero->getCurrentCell().isInVision())//if hero is seen
      {
        int ct=0 ;
        int rr = 5 ;
        for(Hero* my_hero : world->getMyHeroes()) {
          if (min_dist > world->manhattanDistance(opp_hero->getCurrentCell(),
                                                  my_hero->getCurrentCell())) {
              min_dist = world->manhattanDistance(opp_hero->getCurrentCell(),
                                                  my_hero->getCurrentCell());
              rr = ct ;
          }
          ct ++ ;
        }
        if(rr==5) continue ;
        int rn , dj ;

        switch (opp_hero->getName()) {
            case HeroName::HEALER:
                rn = 4 ;
                dj = 25 ;
                break;
            case HeroName::BLASTER:
                rn = 5 ;
                dj = 20 ;
                break;
            case HeroName::GUARDIAN:
                rn = 2 ;
                dj = 40 ;
                break;
            case HeroName::SENTRY:
                rn = 7 ;
                dj = 30 ;
                break;
        }

        if(min_dist<=rn)
        {
          damage_taken[rr] += dj ;
        }
      }
  }
}
//////////////ehsan2


pair<int,Cell> calcHEALER(Hero* my_hero ,int& ap , World* world)
{
    srand(time(0));
}

pair<int,Cell> calcGUARDIAN(Hero* my_hero ,int& ap , World* world,int index){
    int maxManInArea = 0;
    Ability abil = my_hero->getAbility(AbilityName::GUARDIAN_ATTACK);
    Map map = world->getMap();

        pair<int,Cell> targetCellForBomb = findBestPosWithRange(my_hero,world->getOppHeroes(), world, abil,maxManInArea);
        targetCellForBomb.first = 1;
        if(maxManInArea > 0){
            damage_deal[index] = maxManInArea * abil.getPower();
            ap += abil.getAPCost() ;
            return targetCellForBomb;
        }

    targetCellForBomb.first = -1 ;
    targetCellForBomb.second = Cell::NULL_CELL ;
    damage_deal[index] = 0 ;
    return targetCellForBomb;

}

int calcAP(World* world){
  int ap = 0 ;
  vec.clear() ;
  for(int index = 0; index < world->getMyHeroes().size() ;index ++) {
    Hero* hero = world->getMyHeroes()[index];
      switch (hero->getName()) {
          case HeroName::HEALER:
              vec.push_back(calcHEALER(hero, ap, world));
              break;
          case HeroName::BLASTER:
              vec.push_back(calcBLASTER(hero, ap, world,index));
              break;
          case HeroName::GUARDIAN:
              vec.push_back(calcGUARDIAN(hero, ap, world,index));
              break;
      }
  }
  return ap ;
}
///////////////////////////////////////////////////


int const N = 111;
static int f[N][N] , fp[N][N];
static pair<int, int> destination[11];
static vector<Direction> directions[11];


void printMap(World* world){
    Map map = world->map();
    for (int row = 0; row < map.getRowNum(); row++) {
        for (int column = 0; column < map.getColumnNum(); column++) {
            Cell cell = map.getCell(row, column);
            char cur;
            if (world->getMyHero(row, column) != Hero::NULL_HERO)
                cur = static_cast<char>('0' + world->getMyHero(row, column).getId() % 10);
            else if (world->getOppHero(row, column) != Hero::NULL_HERO)
                cur = static_cast<char>('0' + world->getOppHero(row, column).getId() % 10);
            else if (cell.isWall())
                cur = '#';
            else if (cell.isInVision())
                cur = '+';
            else
                cur = '.';
            cerr << cur << ' ';

        }
        cerr << endl;
    }
}

bool inside_map(int n, int m, int r, int c) {
    return r >= 0 && r < n && c >= 0 && c < m;
}

vector<Cell *> give_friend_positions(World *world, Hero* hero, bool near) {
    Map map = world->getMap();
    vector<Cell *> ret;

    for (Hero* hero_friend : world->getMyHeroes()) {
        if(hero_friend == hero)
            continue;
        int cur_row = hero_friend->getCurrentCell().getRow();
        int cur_column = hero_friend->getCurrentCell().getColumn();

        if (!near) {
            Cell cell = world->getMap().getCell(cur_row, cur_column);
            ret.push_back(&cell);

        }else {
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    int dis = abs(dr) + abs(dc);
                    if (dis > 1)
                        continue;

                    int r = cur_row + dr;
                    int c = cur_column + dc;

                    Cell cell = world->getMap().getCell(r, c);
                    ret.push_back(&cell);
                }
            }
        }

    }
    return ret;
}

void calc_f(World* world, Hero* hero){
    Map map = world->getMap();
    int n = map.getRowNum();
    int m = map.getColumnNum();

    bool istank = (hero->getName() == HeroName::GUARDIAN);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            f[i][j] = fp[i][j];
        }
    }

    // near friend
    for (Hero* hero_friend : world->getMyHeroes()) {
        if (hero == hero_friend)
            continue;
        int friend_row = hero_friend->getCurrentCell().getRow();
        int friend_column = hero_friend->getCurrentCell().getColumn();
        for (int dr = -2; dr <= 2; dr++) {
            for (int dc = -2; dc <= 2; dc++) {

                int dis_with_friend = abs(dr) + abs(dc);
                if(dis_with_friend > 2)
                    continue;

                int r = friend_row + dr;
                int c = friend_column + dc;
                if(!inside_map(map.getRowNum(), map.getColumnNum(), r, c))
                    continue;

                f[r][c] -= (3 - dis_with_friend) * 20000000;
            }
        }
    }

    // near other destinations
    for (Hero* hero_friend : world->getMyHeroes()) {
        if (hero == hero_friend)
            continue;
        int id = hero_friend->getId();
        int friend_dest_row = destination[id].first;
        int friend_dest_column = destination[id].second;

        for (int dr = -2; dr <= 2; dr++) {
            for (int dc = -2; dc <= 2; dc++) {

                int dis_with_friend = abs(dr) + abs(dc);
                if(dis_with_friend > 2)
                    continue;

                int r = friend_dest_row + dr;
                int c = friend_dest_column + dc;
                if(!inside_map(map.getRowNum(), map.getColumnNum(), r, c))
                    continue;

                f[r][c] -= (3 - dis_with_friend) * 1000000;
            }
        }

    }

    // near enemy
    int const INF = 1111;
    int fl[N][N];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            fl[i][j] = INF;


    for (Hero *hero_enemy : world->getOppHeroes()) {

        int enemy_row = hero_enemy->getCurrentCell().getRow();
        int enemy_column = hero_enemy->getCurrentCell().getColumn();

        for (int dr = -5; dr <= 5; dr++) {
            for (int dc = -5; dc <= 5; dc++) {

                int dis_with_enemy = abs(dr) + abs(dc);
                if (dis_with_enemy > 5)
                    continue;

                int r = enemy_row + dr;
                int c = enemy_column + dc;
                if (!inside_map(map.getRowNum(), map.getColumnNum(), r, c))
                    continue;

                fl[r][c] = min(fl[r][c], dis_with_enemy);
                // f[r][c] += 100000;
                // f[r][c] += (5 - abs(4 - dis_with_enemy)) * 10000;
            }
        }
    }

    int range = (istank ? 1 : 4);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (fl[i][j] <= range) {
                f[i][j] += 100000;
                f[i][j] += fl[i][j] * 10000;
            }
        }
    }



    // near to hero
    /*
    for (int row = 0; row < n; row++) {
        for (int column = 0; column < m; column++) {

            Cell cell = map.getCell(row, column);
            int hero_row = hero->getCurrentCell().getRow();
            int hero_column = hero->getCurrentCell().getColumn();

            vector<Cell *> friend_positions;
            vector<Direction> directions;

            //auto friend_positions = give_friend_positions(world, hero , true);
            //auto directions = world->getPathMoveDirections(hero_row, hero_column, row, column, friend_positions);

            if (directions.empty()) {
                friend_positions = give_friend_positions(world, hero , false);
                directions = world->getPathMoveDirections(hero_row, hero_column, row, column, friend_positions);
            }
            if (directions.empty()) {
                directions = world->getPathMoveDirections(hero_row, hero_column, row, column);
            }
            auto dis = directions.size();


            //int dis = world->manhattanDistance(hero_row, hero_column, row, column);

            f[row][column] += dis * 10;
        }
    }
    */

}

Cell dodge_place(World* world, Hero* hero) {
    calc_f(world, hero);

    Map map = world->getMap();
    int n = map.getRowNum();
    int m = map.getColumnNum();

    int cur_x = hero->getCurrentCell().getRow();
    int cur_y = hero->getCurrentCell().getColumn();

    int r;
    bool istank = false;
    if (hero->getName() == HeroName::GUARDIAN)
        r = 2 , istank = true;
    else
        r = 4;

    int mx = -1000000000;
    pair<int , int> ret = {0, 0};
    for (int dx = -r; dx <= r; dx++) {
        for (int dy = -r; dy <= r; dy++) {
            int x = cur_x + dx;
            int y = cur_y + dy;
            int dis = abs(dx) + abs(dy);

            if((dis < 3 || dis > 5) && !istank)
                continue;
            if(istank && dis <= 1) continue;
            if(!inside_map(n, m, x, y))
                continue;

            if(f[x][y] > mx){
                mx = f[x][y];
                ret = {x, y};
            }
        }
    }

    return map.getCell(ret.first, ret.second);
}




void AI::preProcess(World *world) {
    Map map = world->getMap();
    int n = map.getRowNum();
    int m = map.getColumnNum();

    // wall
    for (int row = 0; row < n; row++) {
        for (int column = 0; column < m; column++) {
            Cell cell = map.getCell(row, column);
            if (cell.isWall()) {
                fp[row][column] -= 1000000000;
            }
        }
    }


    // objective zone
    for (int row = 0; row < n; row++) {
        for (int column = 0; column < m; column++) {
            Cell cell = map.getCell(row, column);
            if (cell.isInObjectiveZone()) {
                fp[row][column] += 100000000;
            }
        }
    }
}

void AI::pick(World *world) {
    //world->pickHero(HeroName::BLASTER);

    cerr << "-pick" << endl;
    static int cnt= 0;

    switch(cnt){
        case 0:
            world->pickHero(HeroName::BLASTER);
            break;
        case 1:
            world->pickHero(HeroName::BLASTER);
            break;
        case 2:
            world->pickHero(HeroName::BLASTER);
            break;
        case 3:
            world->pickHero(HeroName::GUARDIAN);
            break;
        default:
            world->pickHero(HeroName::BLASTER);
    }
    cnt++;

}


void AI::move(World *world) {
    Map map = world->getMap();
    int n = map.getRowNum();
    int m = map.getColumnNum();

    int needing = calcAP(world);

    // ordering
    vector<Hero*> order;

    for (Hero* hero : world->getMyHeroes())
        if (hero->getName() == HeroName::GUARDIAN)
            order.push_back(hero);

    for (Hero* hero : world->getMyHeroes())
        if (hero->getName() != HeroName::GUARDIAN)
            order.push_back(hero);





    // calc destination
    if (world->getMovePhaseNum() % 1 == 0) {

        for (Hero *hero : order) {
            Cell cell = hero->getCurrentCell();
            pii cur_cell = {cell.getRow(), cell.getColumn()};
            int id = hero->getId();

            if (cur_cell.first == -1)
                continue;

            calc_f(world, hero);

            vector<pair<int, pii> > f_vec;
            for (int row = 0; row < n; row++) {
                for (int column = 0; column < m; column++) {
                    f_vec.push_back({f[row][column], {row, column}});
                }
            }

            sort(f_vec.begin(), f_vec.end());
            destination[id] = f_vec[f_vec.size() - 1].second;
            cerr << cur_cell.first << ',' << cur_cell.second << " ---> " << destination[id].first << ','
                 << destination[id].second << endl;


        }
    }



    // ordering
    order.clear();

    for (Hero* hero : world->getMyHeroes())
        if (hero->getName() == HeroName::GUARDIAN)
            order.push_back(hero);

    vector<pair<int, Hero*> > tmp;
    for (Hero* hero : world->getMyHeroes()) {
        if (hero->getName() == HeroName::GUARDIAN)
            continue;

        Cell cell = hero->getCurrentCell();
        pii cur_cell = {cell.getRow(), cell.getColumn()};
        int id = hero->getId();

        if(cur_cell.first == -1)
            continue;

        int dif_with_dest = f[destination[id].first][destination[id].second] - f[cur_cell.first][cur_cell.second];
        tmp.push_back({dif_with_dest, hero});
    }

    sort(tmp.begin(), tmp.end());
    for (int i = tmp.size() - 1; i >= 0; i--) {
        order.push_back(tmp[i].second);
    }



    // move
    for (Hero* hero : order) {

        Cell cell = hero->getCurrentCell();
        pii cur_cell = {cell.getRow(), cell.getColumn()};
        int id = hero->getId();

        if (cur_cell.first == -1)
            continue;

        calc_f(world, hero);

        cerr << destination[id].first << " , " << destination[id].second << endl;

        int dif_with_dest = f[destination[id].first][destination[id].second] - f[cur_cell.first][cur_cell.second];

        // not a better destination
        if(dif_with_dest < 90000)
            continue;

        auto friend_positions = give_friend_positions(world, hero, true);
        directions[id] = world->getPathMoveDirections(cur_cell.first, cur_cell.second, destination[id].first,
                                                      destination[id].second, friend_positions);

        if (directions[id].empty()) {
            friend_positions = give_friend_positions(world, hero, false);
            directions[id] = world->getPathMoveDirections(cur_cell.first, cur_cell.second, destination[id].first,
                                                          destination[id].second, friend_positions);
        }

        if (directions->empty()) {
            directions[id] = world->getPathMoveDirections(cur_cell.first, cur_cell.second, destination[id].first,
                                                          destination[id].second);
        }

        cerr << "Heros ID is " << id << endl;
        cerr << "SIZE OF PATH FROM " << cur_cell.first << ',' << cur_cell.second << " TO " << destination[id].first
             << ',' << destination[id].second << " IS " <<  directions[id].size() << endl;

        //for (auto dir : directions[id]) {
            auto dir = directions[id][0];

            pii next_cell = cur_cell;


            cerr << "MOVE COMMAND TOWARD ";
            switch (dir) {
                case Direction::DOWN:
                    cerr << "Down\n";
                    next_cell.first++;
                    break;
                case Direction::UP:
                    cerr << "Up\n";
                    next_cell.first--;
                    break;
                case Direction::RIGHT:
                    cerr << "Right\n";
                    next_cell.second++;
                    break;
                case Direction::LEFT:
                    cerr << "Left\n";
                    next_cell.second--;
                    break;
            }

            // not a good place too move
            if (f[cur_cell.first][cur_cell.second] - f[next_cell.first][next_cell.second] > 90000000)
                continue;

            if(world->getAP() - needing - hero->getMoveAPCost() >= 0) {
                world->moveHero(hero->getId(), dir);
            }

            //break;
        //}

    }
}


void AI::action(World *world) {
    cerr << "-action" << endl;
    calcAP(world) ;
    calcDamageTaken(world) ;
    int cnt = 0 ;
    bool dd[10] ;
    for(int i=0;i<10;i++)
      dd[i] = false ;

    for(int i = 0 ; i < world->getMyHeroes().size() ;i++) {
        Hero* my_hero = world->getMyHeroes()[i];

        if(my_hero->getName() == HeroName::BLASTER)
        {
          Ability abil = my_hero->getAbility(AbilityName::BLASTER_DODGE) ;

          if(damage_taken[cnt] > damage_deal[cnt] || (damage_taken[i] >= my_hero->getCurrentHP() && damage_taken[cnt] == damage_deal[cnt]))
          {
            if(abil.isReady() && !dd[cnt])
            {
              world->castAbility(*my_hero, AbilityName::BLASTER_DODGE, dodge_place(world,my_hero) );
              dd[cnt] = true ;
            }

          }

          if(vec[cnt].first == 0)
            world->castAbility(*my_hero, AbilityName::BLASTER_BOMB, vec[cnt].second);
          else if(vec[cnt].first == 1)
            world->castAbility(*my_hero, AbilityName::BLASTER_ATTACK, vec[cnt].second);
        }
        else {
            if (damage_taken[cnt] > damage_deal[cnt]) {
                Ability abil = my_hero->getAbility(AbilityName::GUARDIAN_DODGE);
                if (abil.isReady() && !dd[cnt]) {
                    world->castAbility(*my_hero, AbilityName::GUARDIAN_DODGE, dodge_place(world, my_hero));
                    dd[cnt] = true;
                }
            }

            Ability ability = my_hero->getAbility(AbilityName::GUARDIAN_FORTIFY);
            int ct2 = -1;
            int ttpp = 7 ;
            int max_damage_taken = 0;
            Cell targetf = Cell::NULL_CELL;

            for (Hero *me_hero : world->getMyHeroes()) {
                ct2++;
                if (dd[ct2] || damage_taken[ct2] == 0 || world->manhattanDistance(me_hero->getCurrentCell(),
                                                                                  my_hero->getCurrentCell()) >
                                                         ability.getRange())
                    continue;
                if (max_damage_taken >= damage_taken[ct2])
                    continue;
                max_damage_taken = damage_taken[ct2];
                targetf = me_hero->getCurrentCell();
                ttpp = ct2 ;
            }
            if (targetf != Cell::NULL_CELL) {
                dd[ttpp] = true ;
                world->castAbility(*my_hero, AbilityName::GUARDIAN_FORTIFY, targetf);
            }

            if (vec[cnt].first == 1)
                world->castAbility(*my_hero, AbilityName::GUARDIAN_ATTACK, vec[cnt].second);
        }
        cnt ++ ;
    }

//    printMap(world);
}

#include <stdlib.h>
#include "AI.h"

using namespace std;
typedef pair<int, int> pii;
typedef pair<pii, int> ppi;


static vector<Cell>enpos[10] ;
static vector<int>enval[10] ;
static int ft[10] ;
static int val[32][32], seen[32][32];

///////////////////////////// sina

int const N = 111, INF = 1e9+10;
static bool safe[N][N];
pii bomber_dest[11];
bool bomber_good_places[N][N];
int bomber_f[N][N];



bool inside_map(int n, int m, int r, int c) {
    return c >= 0 && c < m && r >= 0 && r < n;
}
static const int dr[5] = {-1, 0, 1, 0};
static const int dc[5] = {0, 1, 0, -1};
vector<ppi> give_dis(vector<pii> starting_cells, Map &map, bool is_wall_avoided = true, int max_distance = 100
        , vector<pii> avoiding_cells = vector<pii>()) {
    queue<pii> q;
    int n = map.getRowNum(), m = map.getColumnNum();
    int dis[N][N];
    bool is_avoiding[N][N];

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            dis[i][j] = INF, is_avoiding[i][j] = false;

    for (pii x : avoiding_cells)
        is_avoiding[x.first][x.second] = true;

    if (is_wall_avoided)
        for (int r = 0; r < n; r++)
            for (int c = 0; c < m; c++)
                if (map.getCell(r, c).isWall())
                    is_avoiding[r][c] = true;

    for (pii x : starting_cells)
        q.push(x), dis[x.first][x.second] = 0;

    while (!q.empty()) {
        int r = q.front().first;
        int c = q.front().second;
        q.pop();
        if (dis[r][c] == max_distance)
            continue;

        for (int i = 0; i < 4; i++) {
            int new_r = r + dr[i], new_c = c + dc[i];
            if (!inside_map(n, m, new_r, new_c) || dis[new_r][new_c] <= dis[r][c] + 1 || is_avoiding[new_r][new_c])
                continue;
            q.push({new_r, new_c});
            dis[new_r][new_c] = dis[r][c] + 1;
        }
    }

    vector<ppi> ret;
    for (int r = 0; r < n; r++)
        for (int c = 0; c < m; c++)
            if (dis[r][c] != INF)
                ret.push_back({{r, c}, dis[r][c]});

    return ret;
}


Cell findClosestObj(World* world, Cell &cell) {
    Map map = world->getMap();
    int r = cell.getRow(), c = cell.getColumn();
    vector<ppi> near = give_dis({{r, c}}, map);

    int mn = INF, mn_r = r, mn_c = c;
    for (ppi a : near) {
        int r = a.first.first, c = a.first.second;
        int d = a.second;
        if (d < mn && map.getCell(r, c).isInObjectiveZone()) {
            mn = d;
            mn_r = r;
            mn_c = c;
        }
    }
    return map.getCell(mn_r, mn_c);
}


void go(World* world, Cell &a , Cell &b, int id) {
    vector<Direction> dirs = world->getPathMoveDirections(a, b);

    if (b.isWall()) {
        cout << "DEST IS WALL" << endl;
    }

    if (a == b) {
        cout << "ITS THE SAME" << endl;
    }

    if (dirs.empty()) {
        cout << "DIR IS EMPTY" << endl;
        return;
    }

    if (world->getAP() - world->getHero(id).getMoveAPCost() >= 20)
        world->moveHero(id, dirs[0]);
}

void bomber_calc_f(int id, World* world) {
    Map map = world->getMap();
    int n = map.getRowNum();
    int m = map.getColumnNum();

    for (int r = 0; r < n; r++)
        for (int c = 0; c < m; c++)
            bomber_f[r][c] = 0;

    for (Hero* hero : world->getMyHeroes()) {
        if (hero->getName() != HeroName::BLASTER)
            continue;
        if (id == hero->getId())
            continue;
        if (hero->getCurrentCell().getRow() == -1 || bomber_dest[id].first == -1)
            continue;

        vector<ppi> near = give_dis({bomber_dest[hero->getId()]}, map, false, 5);
        for (ppi a : near) {
            int r = a.first.first, c = a.first.second;
            bomber_f[r][c] -= 1000;
        }
    }

    int r = world->getHero(id).getCurrentCell().getRow();
    int c = world->getHero(id).getCurrentCell().getColumn();
    vector<ppi> near = give_dis({{r, c}}, map);
    for (ppi a : near) {
        int r = a.first.first, c = a.first.second;
        int d = a.second;
        bomber_f[r][c] += 100 - d;
    }
}

///////////////////////// /sina



static int sid ;
static bool goright = false ;
static Cell right_dest , left_dest ;
static bool is_safe = true ;
static bool sscouted = false;

AbilityConstants* findAbility(AbilityName an , World* world)
{
  auto vec = world->getAbilityConstants() ;
  for(auto u:vec)
  {
    if(u->getName() == an)
      return u;
  }
}

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

void AI::preProcess(World *world) {
    Map map = world->getMap();
    int n = map.getRowNum();
    int m = map.getColumnNum();

    // Choose Left
    for(int i=2;i<30;i++)
    {
      vector<Cell>vec ;
      for(int j=0;j<32;j++)
      {
        Cell c = map.getCell(j,i) ; // ?
        if(c.isInObjectiveZone() && !c.isWall())
          vec.push_back(c) ;
      }
      if(vec.size())
      {
        Cell c = vec[vec.size()/2] ;
        left_dest = c ;
        break;
      }
    }

    // Choose Right
    for(int i=30;i>2;i--)
    {
      vector<Cell>vec ;
      for(int j=0;j<32;j++)
      {
        Cell c = map.getCell(j,i) ; // ?
        if(c.isInObjectiveZone())
          vec.push_back(c) ;
      }
      if(vec.size())
      {
        Cell c = vec[vec.size()/2] ;
        right_dest = c ;
        break;
      }
    }



    /////////////////////// sina

    for (int i = 0; i < 11; i++) {
        bomber_dest[i] = {-1, -1};
    }

    // safe
    bool up = false;
    for (int r = 0; r < n; r++) {
        for (int c = 0; c < m; c++) {
            Cell cell = map.getCell(r, c);
            if (cell.isInMyRespawnZone()) {
                if (r < n/2)
                    up = true;
                else
                    up = false;
            }
        }
    }

    for (int r = 0; r < n; r++) {
        for (int c = 0; c < m; c++) {
            Cell cell = map.getCell(r, c);
            if (cell.isWall())
                continue;
            if (r < n/2 && up) {
                if (map.getCell(r+1, c).isWall()) {
                    safe[r][c] = true;
                }
            }else if(r >= n/2 && !up){
                if (map.getCell(r-1, c).isWall()) {
                    safe[r][c] = true;
                }
            }

        }
    }



    // bomber places

    int min_row = INF, max_row = -1;
    for (int r = 0; r < n; r++) {
        for (int c = 0; c < m; c++) {
            if (map.getCell(r, c).isInObjectiveZone()) {
                min_row = min(min_row, r);
                max_row = max(max_row, r);
            }
        }
    }

    for (int r = 0; r < n; r++) {
        for (int c = 0; c < m; c++) {
            if (!safe[r][c] || map.getCell(r, c).isInObjectiveZone())
                continue;

            int dis_with_obj = (r < n/2 ? min_row - r : r - max_row);
            if (dis_with_obj < 3 || dis_with_obj > 5)
                continue;

            bomber_good_places[r][c] = true;
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
            world->pickHero(HeroName::SENTRY);

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


    ////////////////// sina

    // bomber destination
    for (Hero* hero : world->getMyHeroes()) {
        if (hero->getName() == HeroName::SENTRY)
            continue;
        if (!hero->getCurrentCell().isInMyRespawnZone())
            continue;
        int id = hero->getId();

        bomber_calc_f(id, world);

        vector<pair<int, pii>> vec;
        for (int r = 0; r < n; r++) {
            for (int c = 0; c < m; c++) {
                if (!bomber_good_places[r][c])
                    continue;
                vec.push_back({bomber_f[r][c], {r, c}});
            }
        }
        sort(vec.begin(), vec.end());
        bomber_dest[id] = vec.back().second;
    }



    for(Hero* enemy:world->getOppHeroes())
    {
        if(enemy->getCurrentCell().isInVision() == false)
            continue ;
        int eid = enemy->getId() ;
        Cell cen = enemy->getCurrentCell() ;
        Cell cs = world->getHero(sid).getCurrentCell() ;
        if(world->isInVision(cen,cs))
        {
            sscouted = true ;
        }
        enpos[eid].clear() ;
        enval[eid].clear() ;

        if(enemy->getName() == HeroName::BLASTER)
        {
            ft[eid] = 5 ;
            enpos[eid].push_back(cen) ;
            enval[eid].push_back(1000) ;
        }
        else
        {
            ft[eid] = 1;
            for(int i=0;i<32;i++)
            {
                for(int j=0;j<32;j++)
                {
                    Cell c = map.getCell(i,j) ;
                    int dd = world->manhattanDistance(c , cen) ;
                    if(c.isWall()) continue ;
                    if(dd > 5) continue ;
                    int val22[6] = {250,50,50,50,80,80} ;
                    enpos[eid].push_back(c) ;
                    enval[eid].push_back(val22[dd]) ;
                }
            }
        }
    }


    cout << "LINE 1" << endl;

    for(Hero* hr:world->getMyHeroes())
    {
      if(hr->getName() == HeroName::SENTRY)
      {
        sid = hr->getId() ;
        is_safe = hr->getAbility(AbilityName::SENTRY_DODGE).isReady() ;
      }
    }

    cout << "LINE 2" << endl;


    int phase = world->getMovePhaseNum();
    if(phase == 0)
      sscouted = false;


    cout << "LINE 3" << endl;
/*
    if (world->getMovePhaseNum() == 0)
    {
      cout <<" ### "<< endl ;
      auto ca = world->getOppCastAbilities();
      for(auto u : ca)
      {
        int id = u->getCasterId() ;
        AbilityName an = u->getAbilityName() ;
        if(id<0 || id>10)
          continue ;
        if(an == AbilityName::BLASTER_DODGE ||
          an == AbilityName::GUARDIAN_DODGE ||
        an == AbilityName::SENTRY_DODGE ||
      an == AbilityName::HEALER_DODGE)
        dodge_cd[id] = findAbility(an , world)->getCooldown() ;
        if(an == AbilityName::BLASTER_BOMB ||
          an == AbilityName::GUARDIAN_FORTIFY ||
        an == AbilityName::SENTRY_RAY ||
      an == AbilityName::HEALER_HEAL)
        ult_cd[id] = findAbility(an , world)->getCooldown() ;
      }
      for(int i=0;i<10;i++)
        ult_cd[i]-- , dodge_cd[i]--;
    }*/

    for(Hero* enemy:world->getOppHeroes())
    {
      if(enemy->getCurrentCell().isInVision() == false)
        continue ;
      Cell cen = enemy->getCurrentCell() ;
      Cell cs = world->getHero(sid).getCurrentCell() ;
      if(world->isInVision(cen,cs))
      {
        sscouted = true ;
      }
    }

    if(phase==0)
      return ;


    cout << "LINE 4" << endl;

//    Hero sry = world->getHero(sid) ;
    Cell dest = Cell::NULL_CELL ;
    Cell cur = world->getHero(sid).getCurrentCell() ;
    if(goright && cur == right_dest)
        goright = false;
    if(!goright && cur == left_dest)
        goright = true;

    if(cur.isInObjectiveZone())
    {
      if(goright)
      {
        dest = right_dest ;
      }
      else
      {
        dest = left_dest ;
      }
    }
    else
    {
      dest = findClosestObj(world , cur) ;
    }

    cout << "LINE 5" << endl;

    int xx = world->getHero(sid).getCurrentCell().getRow() ;
    int yy = world->getHero(sid).getCurrentCell().getColumn() ;
    if(is_safe || ( !is_safe && !safe[xx][yy] ) )
        go(world,cur,dest,sid);

    for(Hero* hr:world->getMyHeroes())
    {

      if(hr->getId() == sid)
        continue ;
      cur = hr->getCurrentCell() ;
      int x = bomber_dest[hr->getId()].first ;
      int y = bomber_dest[hr->getId()].second;
      dest = map.getCell(x,y) ;
      if(cur==dest)
        continue ;
      go(world,cur,dest,hr->getId()) ;
    }

    cout << "LINE 6" << endl;
}

Cell findSafeCell(World* world)
{
  Cell dest ;
  int xx = world->getHero(sid).getCurrentCell().getRow() ;
  int yy = world->getHero(sid).getCurrentCell().getColumn() ;

  for(int i=0;i<32;i++)
  {
    for(int j=0;j<32;j++)
    {
      int dd = world->manhattanDistance(i,j,xx,yy) ;
      if(safe[i][j] && dd>3 && dd<6)
      {
        return world->getMap().getCell(i,j) ;
      }
      if(dd>3 && dd<6)
        dest = world->getMap().getCell(i,j) ;
    }
  }
  return dest ;
}

void AI::action(World *world) {
    cerr << "-action" << endl;

    Map map = world->getMap();
    int n = map.getRowNum();
    int m = map.getColumnNum();

    if(sscouted &&  is_safe)
    {
      Cell dest = findSafeCell(world) ;
      world->castAbility(world->getHero(sid), AbilityName::SENTRY_DODGE, dest);
    }

    for(int i=0;i<32;i++)
    {
        for(int j=0;j<32;j++)
        {
            val[i][j] = seen[i][j] ;
        }
    }

    for(Hero* enemy:world->getOppHeroes())
    {
        int eid = enemy->getId() ;
        if(ft[eid]<=0)
            continue ;
        for(int i=0;i<enpos[eid].size();i++)
        {
            int x = enpos[eid][i].getRow() ;
            int y = enpos[eid][i].getColumn() ;
            val[x][y] += enval[eid][i] ;
        }
    }

    for(Hero* me:world->getMyHeroes())
    {
        if(me->getName() == HeroName::SENTRY)
            continue ;
        Cell cur = me->getCurrentCell() ;
        int x = cur.getRow();
        int y = cur.getColumn() ;
        int mx = 0;
        Cell tar ;
        for(int i=0;i<32;i++)
        {
            for(int j=0;j<32;j++)
            {
                if(world->manhattanDistance(i,j,x,y)>15)
                    continue ;
                int sumval = 0 ;
                for(int di=-5;di<=5;di++)
                {
                    for(int dj=-5;dj<=5;dj++)
                    {
                        if(abs(di)+abs(dj)>5)
                            continue ;
                        int nx = i+di ;
                        int ny = j+dj ;
                        if (!inside_map(n, m, nx, ny))
                            continue;
                        sumval += val[nx][ny] ;

                        cout << "nx ny " << nx << ' ' << ny << endl;
                    }
                }
                if(mx<sumval)
                {
                    mx = sumval ;
                    tar = map.getCell(i,j) ;
                }
            }
        }

        cout << "TAR " << tar.getRow() << ',' << tar.getColumn() << endl;
        world->castAbility(*me, AbilityName::BLASTER_BOMB, tar);
    }


    for(int i=0;i<10;i++)
        ft[i] -- ;


    printMap(world);
}

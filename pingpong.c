#include <curses.h>
#include <sys/types.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define MAX_NAME_LENGTH 10
#define WIN_SCORE 3

typedef struct {
  float x_pos;
  float y_pos;

  int width;
  int height;
} Paddle;

typedef struct {

  char name[MAX_NAME_LENGTH];
  uint16_t score;
  Paddle paddle;

} Player;

typedef struct {
  float x_pos;
  float y_pos;

  float x_dir;
  float y_dir;
} Ball;

typedef struct {

  Player players[2];
  Ball ball;
  int height;
  int width;

  u_char **representation;


} Table;

Table* create_new_table(int height, int width);
void* safe_malloc(size_t size);
void game_loop(WINDOW * window, Table *table);
void set_paddle_attributes(Paddle *p,int x_pos, int y_pos);
Player* get_winner(Table *table);
void print_table(WINDOW *window, Table *table);
void handle_potential_user_input(Table *table);
void move_computer_paddle(Table *table);
int move_ball(Table *table);



int main( int argc, char* argv[] )
{

  if( argc > 2 ) {
    if(strcmp("Multiplayer", argv[1])) {
      printf("Multiplayer has not been implemented");
      return -1;
    }
  }

  WINDOW *window = initscr();

  int terminal_height, terminal_width;
  getmaxyx(window, terminal_height, terminal_width);
  keypad(stdscr, TRUE);
  curs_set(0);

  Table *table = create_new_table(terminal_height, terminal_width);

  timeout(0);

  game_loop( window, table );

  endwin();

  return 0;
}

void game_loop(WINDOW * window, Table *table)
{

  int done = 0;

  while(!done) {

    handle_potential_user_input(table);

    move_computer_paddle(table);

    int went_out = move_ball(table);
    
    if( went_out != 0 ) {
      
      Player *winner = NULL;
      if( went_out < 0 )
        winner = &(table->players[1]);
      else
        winner = &(table->players[0]);
      
      winner->score += 1;
      
      table->ball.x_pos = table->width/2;
      table->ball.y_pos = table->height/2;
      
      if (winner->score >= WIN_SCORE)
        done = 1;
    }


    print_table(window, table);

    refresh();

    usleep(60000);

  }

  //print the winner
  Player *winner = get_winner(table);

  move(table->height/2, table->width/2);
  printw(winner->name);
  printw(" won!");
  refresh();
  sleep(5);
}

void print_paddle(Paddle *p)
{
  int x = p->x_pos, y = p->y_pos;
  
  int i, j;
  for( i = 0; i < p->height; ++i) {
    for( j = 0; j < p->width; ++j) {
      mvaddch((int)y+i, (int)x+j, 0x40);
    }
  }
}

void print_ball( Ball *b )
{
  mvaddch(ceil(b->y_pos), ceil(b->x_pos), 'O');
}

void print_table(WINDOW *window, Table *table)
{
  erase();
  print_paddle( &(table->players[0].paddle) );
  print_paddle( &(table->players[1].paddle) );

  print_ball( &(table->ball) );
}

Player* get_winner(Table *table)
{
  if( table->players[0].score < table->players[1].score ) {
    return &(table->players[1]);
  }
  return &(table->players[0]);
}

Table* create_new_table(int height, int width)
{

  Table *t = safe_malloc(sizeof(Table));

  memset(t, 0, sizeof(Table));

  set_paddle_attributes(&(t->players[0].paddle), 0, height/2);
  set_paddle_attributes(&(t->players[1].paddle), width-1, height/2);

  t->height = height;
  t->width = width;

  t->ball.x_pos = width/2;
  t->ball.y_pos = height/2;
  t->ball.x_dir = 1;
  t->ball.y_dir = 1;

  strncpy(t->players[0].name, "Player 1", MAX_NAME_LENGTH);
  strncpy(t->players[1].name, "Computer", MAX_NAME_LENGTH);

  t->representation = safe_malloc(height);
  int i;
  for( i = 0; i < height; ++i ) {
    t->representation[i] = safe_malloc(width);
    memset(t->representation[i], 0, width);
  }

  return t;
}

void set_paddle_attributes(Paddle *p,int x_pos, int y_pos)
{
  p->width = 1;
  p->height = 4;
  p->x_pos = x_pos;
  p->y_pos = y_pos;
}

void* safe_malloc(size_t size)
{
  void* mem = malloc(size);
  if( mem == NULL ) {
    fprintf( stderr, "Malloc of size %zx failed!\n", size);
    exit(-1);
  }
  return mem;
}


void handle_potential_user_input(Table *table)
{
	int input;
	while((input = getch()) != ERR) {
		if( input == KEY_UP ) {
			table->players[0].paddle.y_pos--;
		} else if( input == KEY_DOWN) {
			table->players[0].paddle.y_pos++;
		}
	}
}


void move_computer_paddle(Table *table)
{
  int ball_y = table->ball.y_pos;

  Paddle* p = &(table->players[1].paddle);

  if( p->y_pos+(p->height/2) < ball_y )
    p->y_pos += 1.2;
  else if( p->y_pos+(p->height/2) > ball_y)
    p->y_pos -= 1.2;

}

int paddle_occupy(Paddle *p, int x, int y)
{
  if(p->x_pos != x)
    return 0;
  return (p->y_pos <= y) && (y <= p->y_pos+p->height);
}

int is_paddle_present(Table *table, int x, int y)
{
  
  Paddle *p1 = &table->players[0].paddle;
  Paddle *p2 = &table->players[1].paddle;

  return paddle_occupy(p1,x,y) || paddle_occupy(p2,x,y);
}

int move_ball(Table *table)
{

  Ball *b = &table->ball;
  
  int next_x = ceil(b->x_pos + b->x_dir), next_y = ceil(b->y_pos + b->y_dir);

  if (is_paddle_present(table, next_x, next_y)) {
    b->x_dir *= -1;
  }

  if( b->y_pos >= table->height || b->y_pos <= 0)
    b->y_dir *= -1;

  if( b->x_pos <= 0)
    return -1; //Went out on the left side
  else if ( b->x_pos >= table->width)
    return 1; //Went out on the right side

  b->x_pos += b->x_dir;
  b->y_pos += b->y_dir;
  return 0;
}

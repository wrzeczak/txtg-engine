#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ncurses.h>

#define X_AXIS 2

#define DEFAULT_TEXT_COLOR 254
#define SYSTEM_MESSAGE_COLOR 255

#define print_message(M) __print_message(M, &__line_count)

//------------------------------------------------------------------------------

void werror(const char * message, int code) {
	endwin();
	system("\nreset\n");
	system("clear");
	fprintf(stderr, "WRZECZAK SYSTEM ERROR [%02d]: %s.\n", code, message);
	exit(code);
}

void todo() {
	werror("Feature is not yet implemented", -64);
}

//------------------------------------------------------------------------------

typedef struct {
	char * name;
	int id;
	int color_pair;
	// a union of a set of attributes, perhaps
} Character;

//------------------------------------------------------------------------------

typedef struct {
	char ** fields;
	int * color_pairs;
	int field_count;
	int format;
} Message;

// message types
enum {
	CHARACTER_MESSAGE = 64,
	SYSTEM_MESSAGE,
	STORY_MESSAGE
};

//------------------------------------------------------------------------------

Character make_character(char * __name, int id, int color) {
	char * name = malloc(strlen(__name) + 1);
	strcpy(name, __name);

	int color_pair_id = 16 + id;

	init_pair(color_pair_id, color, COLOR_BLACK);
	return (Character) { name, id, color_pair_id };
}

void destroy_character(Character * c) {
	free(c->name);
}

//------------------------------------------------------------------------------

Message make_character_message(Character c, char * message) {
	char ** fields = malloc(2 * sizeof(char *));
	fields[0] = malloc(strlen(c.name) + 1);
	fields[1] = malloc(strlen(message) + 1);

	strcpy(fields[0], c.name);
	strcpy(fields[1], message);

	int * color_pairs = malloc(2 * sizeof(int));
	color_pairs[0] = c.color_pair;
	color_pairs[1] = DEFAULT_TEXT_COLOR;

	return (Message) {
		fields,
		color_pairs,
		2,
		CHARACTER_MESSAGE
	};
}

Message make_system_message(char * message) {
	char ** fields = malloc(sizeof(char *));
	fields[0] = malloc(strlen(message) + 1);

	strcpy(fields[0], message);

	return (Message) {
		fields,
		NULL,
		1,
		SYSTEM_MESSAGE
	};
}

Message make_story_message(char * message) {
	char ** fields = malloc(sizeof(char *));
	fields[0] = malloc(strlen(message) + 1);

	strcpy(fields[0], message);

	return (Message) {
		fields,
		NULL,
		1,
		STORY_MESSAGE
	};
}

void destroy_message(Message * m) {
	switch(m->format) {
		case CHARACTER_MESSAGE:
			free(m->fields[0]);
			free(m->fields[1]);
			free(m->fields);

			free(m->color_pairs);
			break;
		case STORY_MESSAGE:
		case SYSTEM_MESSAGE:
			free(m->fields[0]);
			free(m->fields);
			break;
		default:
			todo();
			break;
	}
}

//------------------------------------------------------------------------------

void __print_message(Message m, int * ln) {
	switch(m.format) {
		case CHARACTER_MESSAGE:
			if(m.field_count > 2 || m.field_count < 2) werror("Message constructed with incorrect format", -65);

			attron(COLOR_PAIR(m.color_pairs[0]) | A_UNDERLINE);
				mvprintw(*(ln), X_AXIS, "%s", m.fields[0]);
			attroff(COLOR_PAIR(m.color_pairs[0]));
			attroff(A_UNDERLINE);

			attron(COLOR_PAIR(DEFAULT_TEXT_COLOR));
				mvprintw(*(ln), X_AXIS + 2 + strlen(m.fields[0]), "\"%s\"", m.fields[1]);
			attroff(COLOR_PAIR(DEFAULT_TEXT_COLOR));

			// final: < NAME > "message"
			destroy_message(&m);
			break;
		case SYSTEM_MESSAGE:
			if(m.field_count > 1 || m.field_count < 1) werror("Message constructed with incorrect format", -65);
			attron(COLOR_PAIR(SYSTEM_MESSAGE_COLOR));
				mvprintw(*(ln), X_AXIS, "SYSTEM: %s", m.fields[0]);
			attroff(COLOR_PAIR(SYSTEM_MESSAGE_COLOR));

			destroy_message(&m);
			break;
		case STORY_MESSAGE:
			if(m.field_count > 1 || m.field_count < 1) werror("Message constructed with incorrect format", -65);

			attron(COLOR_PAIR(DEFAULT_TEXT_COLOR) | A_ITALIC);
				mvprintw(*(ln), X_AXIS, "%s... ", m.fields[0]);
			attroff(COLOR_PAIR(DEFAULT_TEXT_COLOR));
			attroff(A_ITALIC);

			destroy_message(&m);
			break;
		default:
			todo();
			break;
	}

	*(ln) += 1;
}

//------------------------------------------------------------------------------

int main(void) {
	initscr();
	raw();
	cbreak();
	keypad(stdscr, true);

	start_color();

	init_pair(DEFAULT_TEXT_COLOR, COLOR_WHITE, COLOR_BLACK);
	init_pair(SYSTEM_MESSAGE_COLOR, COLOR_YELLOW, COLOR_BLACK);

	box(stdscr, 0, 0);

	int __line_count = 2;

	Character c = make_character("Test", 0, COLOR_RED);

	Message t = make_character_message(c, "This is a test message");

	print_message(t);

	Message s = make_system_message("This is a system message");

	print_message(s);

	Message v = make_story_message("This is a story message");

	print_message(v);

	getch();

	endwin();

	system("\nreset\n");
	system("clear");

	destroy_character(&c);

	return 0;
}


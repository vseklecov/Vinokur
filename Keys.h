#ifndef __KEYS_H__
#define __KEYS_H__

// Наименование клавиш пульта управления
#define KEY_NONE    ('\0')
#define KEY_ON      ('@')
#define KEY_MODE    ('M')
#define KEY_SOUND   ('\e')
#define KEY_PLAY    ('P')
#define KEY_PREV    ('S')
#define KEY_NEXT    ('N')
#define KEY_EQ      ('Q')
#define KEY_MINUS   ('-')
#define KEY_PLUS    ('+')
#define KEY_ARROW   ('\b')
#define KEY_U_SD    ('\n')
#define KEY_0       ('0')
#define KEY_1       ('1')
#define KEY_2       ('2')
#define KEY_3       ('3')
#define KEY_4       ('4')
#define KEY_5       ('5')
#define KEY_6       ('6')
#define KEY_7       ('7')
#define KEY_8       ('8')
#define KEY_9       ('9')

// Управление процессами и экранами
#define PROCESS_START       KEY_PLAY
#define PROCESS_CANCEL      KEY_ON
#define PROCESS_PREV_STATE  KEY_PREV
#define PROCESS_NEXT_STATE  KEY_NEXT
#define PROCESS_SETTINGS    KEY_EQ


#endif
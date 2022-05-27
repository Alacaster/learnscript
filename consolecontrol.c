#include "consolecontrol.h"
#include "fuwautils.h"
#include "errorwrapper.h"

static HANDLE hconout = 0; // da coconut
static COORD virtualcursor = {0, 0}; //cursorpos gets transformed into this variable and onlx needs to be incremented unless the end of a box is reached or a new line or it stays the same if a scroll box moves this is where text gets printed to the screen in the input loop if all checks pass.
//maybe we need a variable to keep track of input focus!
CHAR_INFO * outputbuf = 0;
COORD consolesize = {0, 0};
//the console is in the positive X positive Y
typedef struct {
    SMALL_RECT coords;
    inputfieldstyle_t type;
    TCHAR * buf;
    int cursorpos;
} rects_t;
rects_t *rects = 0, **pinrects = 0, **poutrects = 0;
static short bottomrectheight = 0; //the lowest line that a text box is on, offset from the top of the console. Always subtract 1 when using cause arrays start at 0. is consoleheight.Y == 3 and bottomrectheight is 0 you want to get 2.
static short cinrects = 0, coutrects = 0;
static int ctotalconsolecharacters = 0; //consolesize squared
static int cmaxconsolecharacters = 0; //largest console can be resized to

static BOOL bitmapoutofdate = 1;
static int8_t * inbitmap, * outbitmap;
static int8_t ** pinklist = 0; //pink was the color of the pixels when i was drawing to invent the algorithm
static int16_t pinkcount = 0;
typedef enum {EMPTY = 0, FILLED, GREY, POSSIBLE_CORNER, USED_CORNER} pixeltype;

void (*initializeConsoleBuffersError)(icbe_t value) = NULL;
icbe_t initializeConsoleBuffers(HANDLE hhconout, COORD sizee){
    hconout = hhconout;
    icbe_t errortype = CONSOLE_ALREADY_INITIALIZED;
    static void * jumponce = &&initialize;
    goto *jumponce;
    initialize:
    COORD size = GetLargestConsoleWindowSize(hhconout);
    cmaxconsolecharacters = size.X*size.Y;
    outputbuf = (CHAR_INFO *)calloc(cmaxconsolecharacters, sizeof(CHAR_INFO));
    rects = (rects_t *)calloc(cmaxconsolecharacters * 2, sizeof(rects_t));
    pinrects = (rects_t **)calloc(cmaxconsolecharacters, sizeof(rects_t **));
    poutrects = (rects_t **)calloc(cmaxconsolecharacters, sizeof(rects_t **));
    consolesize.X = sizee.X; consolesize.Y = sizee.Y;
    ctotalconsolecharacters = sizee.X * sizee.Y;
    bitmapoutofdate = 1;
    if(!((int64_t)outputbuf & (int64_t)rects)){
        errortype = CALLOC_FAILED;
        goto alreadyinitialized;
    }
    jumponce = &&alreadyinitialized;
    return CONSOLE_INITIALIZED_SUCCESS;

    alreadyinitialized: //it's literally right here
    if(!initializeConsoleBuffersError){
            initializeConsoleBuffersError(errortype);
    }
    return errortype;
}
/*
    the printing method is this:
    We already have all text boxes in the char_info array
    so everything gets printed automatically but
    boxes are in their own malloc'd buffers as well.
    scrolling boxes reprint by first skipping the length of the
    scroll position and then getting characters, if there is a
    newline and we have lines left, print whitspace for the rest
    of the line, move down a line and skip the length of the
    scroll position in the buffer. If we reach the end of the box
    and have no more room, skip to the next newline and interpret
    it as normal. Scroll boxes also malloc space for scroll
    position which is 1 int.
    When we are taking input is when we have to reprint boxes.
    boxes decide cursor postion when you click into them,
    if you move into them using ctrl+arrow keys it restores the saved
    cursor position.
    When you type on the end of line in a scroll box it scrolls to
    the right, on a solid box it moves down to the next line.
    A newline instantly moves down to the next line.
    Cursor position is relative to the box, not the console.
    When you type into a box it reprints the line you type
    on only.
    we first have to define the new rect in
*/
int resizeConsoleBuffers(COORD size){ //idk bro
    if(consolesize.X > size.X && consolesize.Y > size.Y){
        goto norealloc;
    }
    //realloc(outputbuf, size.Y*size.X*sizeof(CHAR_INFO));
    bitmapoutofdate = 1;
    norealloc:
}

int writeToField(); //write to any field.
int takeInput();

/*
    all structures are passed as pointers

    SCROLLER, CENTER_JUSTIFY, and SELECTABLE do not affect the 
    dimensions of a field.
    a LINEIN type has a height of 1.
    a BOXIN type has any height.

    if MORE_FORMAT is not defined LINEIN and BOXIN start
    at the bottom of the lowest box and are full width.

    providing only LINEIN takes no arguments.                          DONE

    providing only BOXIN requires a int for height.                 DONE

    if MORE_FORMAT alone is defined then the function expects a       DONE
    pointer to a SMALL_RECT and these dimensions are used.

    if MORE_FORMAT and LINEIN are defined the function
    expects a COORD and two unsigned ints and will try to place a 
    box of height 1, COORD is contained in the box, the box 
    attepts to be as wide as possible without exceeding the first
    int to the left and the second int to the right from the
    position of the COORD. If a int is negative that direction
    can be infinity.
    if MORE_FORMAT and CENTER_FIELD (BOXIN optional) are defined it expects a            DONE
    SMALL_RECT and tries to center the field horizontally 
    keeping the width and height and top position.

    if CENTER_FIELD & BOXIN is defined it takes a COORD as
    width and height
    if CENTER_FIELD and LINEIN is defined it takes only a short width

    if FILL or (FILL and BOXIN) the function expects only
    a COORD and makes the box as big as possible from that COORD

    if FILL and LINEIN is defined it expects a coord and makes
    the box horizontally as big as possible.

    if FILL and MORE_FORMAT the function finds the largest box
    within a SMALL_RECT
*/

int checkCOORDisnInside(COORD * a, recttype_t recttype){
    typeof(pinrects) prects = recttype ? poutrects : pinrects;
    typeof(cinrects) crects = recttype ? coutrects : cinrects;
    for(int i = 0; i < crects; i++){
        if(((unsigned)a->X - (unsigned)rects->coords.Left) < (rects->coords.Right - rects->coords.Left) || ((unsigned)a->Y - (unsigned)rects->coords.Bottom) < (rects->coords.Top - rects->coords.Bottom)){
            return 1;
        }
    }
    return 0;
}

#define ifa(b) if(a & (b)) //if is any
#define ifn(b) if(!(a & (b))) //if is none
#define ife(b) if(!(a ^ (b))) //if is exactly
#define ifoa(b) if((!(a & ~(b)) && a)) //if is only any
#define ifaom(b) if(!((a & (b)) ^ (b))) //if is all or more
#define ifnoo(b) if((a & (b)) & ((a & (b)) - 1)) //if is not only one
static fbe_t formatBox(_In_ inputfieldstyle_t *A, SMALL_RECT * b, va_list c, recttype_t f){ //TODO check for invaid coordinates such as negatives or too large
    union {
        SMALL_RECT *rect;
        struct {
            COORD *pos;
            short left;
            short right;
        } formatline;
        COORD *fill;
        COORD *WxH;
    } d;
    int width = 0;
    int a = *A & LINEIN | BOXIN | MORE_FORMAT | CENTER_FIELD | FILL;
    SMALL_RECT e = {0,0,0,0};
    #error "Use correctRect to verify formatting and check parameter bounds <-- TODO"
    ifa(LINEIN | BOXIN){ //has LINEIN or BOXIN at least
        ifaom(BOXIN | LINEIN){
                return BAD_INPUT_FIELD_FORMAT;
            } //has either BOXIN or LINEIN
        ifoa(BOXIN | LINEIN){ //has only BOXIN or LINEIN
            if(bottomrectheight >= consolesize.Y){
                return NOT_ENOUGH_ROOM;
            }
            e.Top = (consolesize.X - ++bottomrectheight);
            e.Left = 0;
            e.Right = consolesize.X - 1;
            e.Bottom = e.Top;
            ifa(BOXIN){
                e.Bottom -= va_arg(c, int) - 1;
                if(e.Bottom < 0){
                    return NOT_ENOUGH_ROOM;
                }else if(e.Bottom > e.Top){
                    return BAD_INPUT_FIELD_FORMAT;
                }
                bottomrectheight = e.Bottom;
            } //has BOXIN or LINEIN and something else too
        }else ifa(LINEIN){
            ifnoo(CENTER_FIELD | MORE_FORMAT | FILL){
                return BAD_INPUT_FIELD_FORMAT; //has only LINEIN and one of the above
            }else ifa(CENTER_FIELD){
                width = va_arg(c, typeof(width));
                e.Top = (consolesize.X - ++bottomrectheight);
                e.Bottom = e.Top;
                goto centerfield;
            }else ifa(MORE_FORMAT){ //must overraide all parameters
                d.formatline.pos = va_arg(c, PCOORD);

            }else ifa(FILL){ //must overraide all parameters
                
            }
        }else{ //has BOXIN and some extra stuff
            ifn(MORE_FORMAT | FILL){
                d.WxH = va_arg(c, typeof(d.WxH));
                e.Top = e.Bottom = consolesize.Y - ++bottomrectheight;
                e.Left = 0;
                e.Right = width = d.WxH->X;
                e.Bottom -= d.WxH->Y;
                if(e.Bottom < 0){
                    bottomrectheight--;
                    return NOT_ENOUGH_ROOM;
                }
                goto centerfield;
            } //past this BOXIN has no effect on anything
        }
        goto loadrect;
    }
    // we have no LINEIN
    ifoa(MORE_FORMAT | BOXIN | CENTER_FIELD){
        ifn(MORE_FORMAT){
            return BAD_INPUT_FIELD_FORMAT;
        }
            d.rect = va_arg(c, typeof(d.rect));
            e.Bottom = d.rect->Bottom;
            e.Left = d.rect->Left;
            e.Right = d.rect->Right;
            e.Top = d.rect->Top;
        ifa(CENTER_FIELD){
            width = e.Right - e.Left;
            centerfield:
            if(width > consolesize.X) return NOT_ENOUGH_ROOM;
            if(width < 0) return BAD_INPUT_FIELD_FORMAT;
            e.Left = consolesize.X / 2 - width / 2; //rounds left when parity does not match
            e.Right = e.Left + width - 1;
        }
        goto loadrect;
    }
    ifoa(FILL | BOXIN | MORE_FORMAT){
        ifa(MORE_FORMAT){
            d.rect = va_arg(c, typeof(d.rect));

        }
        //do normal fill
        loadrect:
        b->Top = e.Top; b->Bottom = e.Bottom; b->Left = e.Left; b->Right = e.Right;
        return INPUT_FIELD_CREATION_SUCCESS;
    }
    return BAD_INPUT_FIELD_FORMAT;
}

cife_t createInputField(_In_ inputfieldstyle_t a, _Out_ hinfield_t * b, ...){
    SMALL_RECT box = {0, 0, 0, 0};
    va_list d;
    va_start(d, b);
    formatBox(&a, &box, d);
    va_end(d);
    for(int i = 0; i < cinrects; i++){
        if(isIntersected(&box, &pinrects[i]->coords)){
            return INPUT_FIELD_OVERLAP;
        }
    }
}

void deleteInputField(hinfield_t a){
    //find the feild to delete, a is just a pointer
    //delete it then repack the array
}
void clearAllFields(){
    memset(poutrects, 0, cmaxconsolecharacters);
    memset(pinrects, 0, cmaxconsolecharacters);
}
//make sure the box it's upside down or rightside left.
#define flip(a, b, temp) do{{temp = (a) ^ (b); a ^= temp; b ^= temp;}} while(0)
void correctRect(SMALL_RECT *a, SMALL_RECT *b){
    short temp = 0;
    if(a->Top < a->Bottom)
        flip(a->Top, a->Bottom, temp);
    if(a->Right < a->Left)
        flip(a->Right, a->Left, temp);
    if(b->Top < b->Bottom)
        flip(b->Top, b->Top, temp);
    if(b->Right < b->Left)
        flip(b->Right, b->Left, temp);
}
//switch two variables using a difference map i.e. XOR
//XOR shows the differences, then XOR applies those differences.
//it's imposible for the rectangles to be intersected if the lowest point
//of one box is above the highest point of the other box. 
//The boxes must not have inverted coordinates
int isIntersected(SMALL_RECT *a, SMALL_RECT *b){
    if(a->Top < b->Bottom || a->Left > b->Right || a->Right < b->Left || a->Bottom > b->Top){
        return 0;
    }
    return 1;
}

void editBitRect(SMALL_RECT * b, pixeltype c, recttype_t recttype){ //just draws the four corners of a rect
    typeof(inbitmap) bitmap = recttype ? outbitmap : inbitmap;
    int a = consolesize.X * b->Top;
    *(bitmap + b->Left + a) = c;
    *(bitmap + b->Right + a) = c;
    a = consolesize.X * b->Bottom;
    *(bitmap + b->Left + a) = c;
    *(bitmap + b->Right + a) = c;
}
void drawBitRect(recttype_t recttype){ //redraws the entire bitmap
    static COORD size;
    static void * jumponce = &&initialize;
    goto *jumponce;
    initialize:
    size = E(GetLargestConsoleWindowSize(hconout));
    inbitmap = malloc(cmaxconsolecharacters * sizeof(int8_t));
    outbitmap = malloc(cmaxconsolecharacters * sizeof(int8_t));
    pinklist = calloc(2 * (size.X  + size.Y), sizeof(pinklist));
    jumponce = &&body;
    body:
    typeof(inbitmap) bitmap = recttype ? outbitmap : inbitmap;
    typeof(pinrects) tprects = recttype ? poutrects : pinrects;
    typeof(cinrects) crects = recttype ? coutrects : cinrects;
    memset(bitmap, 0, ctotalconsolecharacters);
    for(int i = 0; i < crects; i++){ //assumes that pinrects and poutrects are a packed array of pointers to rect structures and all invalid pointers are null
        int a = consolesize.X * tprects[i]->coords.Top;
        *(bitmap + tprects[i]->coords.Left + a) = FILLED;
        *(bitmap + tprects[i]->coords.Right + a) = FILLED;
        a = consolesize.X * tprects[i]->coords.Bottom;
        *(bitmap + tprects[i]->coords.Left + a) = FILLED;
        *(bitmap + tprects[i]->coords.Right + a) = FILLED;
    }
}

typedef struct {
    short tl; //top left, how many come from X and go into X
    short tr; //top right, how many come from Y and go into X
    short bl; //bottom left
    short br;
} matrix2x2_t;

void vectorMatrixMul(COORD * v, const matrix2x2_t m){
    v->X = v->X * m.tl + v->Y * m.tr;
    v->Y = v->X * m.bl + v->Y * m.br;
}
#define apvec(_Coord, _Vector) do{_Coord.X += _Vector.X; _Coord.Y += _Vector.Y;}while(0)
#define unapvec(_Coord, _Vector) do{_Coord.X -= _Vector.X; _Coord.Y -= _Vector.Y;}while(0)
enum {UP = 0x1, DOWN = 0x2, LEFT = 0x4, RIGHT = 0x8};
void getBiggestRect(COORD * ina, recttype_t recttype){
    memset(pinklist, 0, 2 * (consolesize.X  + consolesize.Y) * sizeof(*pinklist));
    const COORD originpos = {ina->X, ina->Y};
    COORD looppos = originpos;
    int8_t ** plpos; //pinklist position //this points to a position in an array of int8_t pointers which point to locations of the bitmap
    const matrix2x2_t rotCWmatx = {0, 1, -1, 0}; //rotate clockwise matrix
    const matrix2x2_t rotCCmatx = {0, -1, 1, 0}; //rotate counterclockwise
    COORD vec = {1, 0}; //we start with going to the right in the clockwise direction
    pinkcount = 0; //number of valid corners
    //makesure red pixel is not in a box actually we don't have to do that cause the overlapping check will do ok, we only have to worry about being outside of the console
    typeof(inbitmap) bitmap = recttype ? outbitmap : inbitmap;
    while(++looppos.Y < consolesize.Y && *(bitmap + looppos.X + looppos.Y * consolesize.X) != FILLED);
    looppos.Y--;
    const COORD loopstart = {looppos.X, looppos.Y};
    while(looppos.Y - loopstart.Y | looppos.X - loopstart.X){ //while not back at beginning
        //this looks complicated, but essentially it's boolean logic cause we take the abs of vec so it's always 1, 0 or 0, 1 and then the x and y of ttpos are one of two components which are choosen by multiplying by 1 or 0;
        //this gets the coord of where to start checking
        const COORD premove = {(looppos.X + vec.X), (looppos.Y + vec.Y)}; //move to here if all checks pass, do checks from this location
        if(premove.X >= 0 && premove.Y >= 0 && premove.X < consolesize.X && premove.Y < consolesize.Y)
            goto rotate; //if premove is out of bounds
        COORD searchpos = {premove.X * abs((int)vec.X) + originpos.X * abs((int)vec.Y), premove.Y * abs((int)vec.Y) + originpos.Y * abs((int)vec.X)};
        if(*(bitmap + searchpos.X + searchpos.Y * consolesize.X) == FILLED){
            goto rotate;
        }
        COORD searchvec = vec;
        vectorMatrixMul(&searchvec, rotCCmatx);
        const COORD searchbound = {premove.X + searchvec.X, premove.Y + searchvec.Y};
        //while our position is within the bounds of the console and we have not gotten back to premove yet
        apvec(searchpos, searchvec);
        while(!(searchpos.X - searchbound.X | searchpos.Y - searchbound.Y)){
            if(*(bitmap + searchpos.X + searchpos.Y * consolesize.X) == FILLED){
                goto skipmove; 
            }
            apvec(searchpos, searchvec);
        }
        looppos.X = premove.X; looppos.Y = premove.Y;
        continue;
        rotate:
        vectorMatrixMul(&vec, rotCWmatx);
        skipmove:
        pinklist[pinkcount++] = ((bitmap + looppos.X + looppos.Y * consolesize.X));
        looppos.Y = searchpos.X - searchvec.X; looppos.Y = searchpos.Y - searchvec.Y;
    }
    int max = 0, tempmax;
    SMALL_RECT maxrect;
    for(int i = 0; i < pinkcount; i++){
        if(pinklist[i] == 0) continue;
        short x = (pinklist[i] - bitmap) % consolesize.X;
        short y = (pinklist[i] - bitmap) / consolesize.X;
        SMALL_RECT rect = {x, y, x, y};
        SMALL_RECT vect = {1, 1, 1, 1};
        COORD rectpos = {x, y};
        COORD vectpos = {1, 0};
        while(vect.Bottom | vect.Left | vect.Right | vect.Top){
            SMALL_RECT prerect = {rect.Left + vect.Left, rect.Top + vect.Top, rect.Right + vect.Right, rect.Bottom + vect.Bottom};
            for(int j = 0; j < 4; j++){
                while((rectpos.X >= rect.Left && vectpos.X == -1) || (rectpos.Y <= rect.Top && vectpos.Y == 1) || (rectpos.X <= rect.Right && vectpos.X == 1) || (rectpos.Y >= rect.Bottom && vectpos.Y == -1)){
                    if(*(bitmap + rectpos.X + rectpos.Y * consolesize.X) == FILLED){
                        if(vectpos.X == -1){ vect.Left = 0; continue;}
                        if(vectpos.Y == 1){ vect.Top = 0; continue;}
                        if(vectpos.X == 1){ vect.Right = 0; continue;}
                        if(vectpos.Y == -1){ vect.Bottom = 0; continue;}
                    }
                    apvec(rectpos, vectpos);
                }
                unapvec(rectpos, vectpos);
                vectorMatrixMul(vectpos, rotCWmatx);
            }
            if(vect.Left) rect.Left = prerect.Left;
            if(vect.Top) rect.Top = prerect.Top;
            if(vect.Right) rect.Right = prerect.Right;
            if(vect.Bottom) rect.Bottom = prerect.Bottom;
        }
        tempmax = (rect.Right - rect.Left + 1) * (rect.Top - rect.Bottom + 1);
        if(tempmax > max){
            max = tempmax;
            maxrect.Left = rect.Left; maxrect.Top = rect.Top; maxrect.Right = rect.Right; maxrect.Bottom = rect.Bottom;
        }
        for(int j = i; j < pinkcount; j++){
            if(pinklist[j] == 0) continue;
            x = (pinklist[j] - bitmap) % consolesize.X;
            y = (pinklist[j] - bitmap) / consolesize.X;
            if(x == maxrect.Left || x == maxrect.Right || y == maxrect.Top || y == maxrect.Bottom){
                pinklist[j] = 0;
            }
        }
    }
    #error "unfinished function"
}
void getWidestLine();
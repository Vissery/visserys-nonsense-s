/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     UNARY = 258,
     CASTREF = 259,
     VOID = 260,
     VAR = 261,
     POINT = 262,
     RECT = 263,
     ARRAY = 264,
     OBJECT = 265,
     REFERENCE = 266,
     INT = 267,
     ARGC = 268,
     ARGCNORET = 269,
     THEENTITY = 270,
     THEENTITYWITHID = 271,
     FLOAT = 272,
     BLTIN = 273,
     BLTINNOARGS = 274,
     BLTINNOARGSORONE = 275,
     BLTINONEARG = 276,
     BLTINARGLIST = 277,
     TWOWORDBUILTIN = 278,
     FBLTIN = 279,
     FBLTINNOARGS = 280,
     FBLTINONEARG = 281,
     FBLTINARGLIST = 282,
     RBLTIN = 283,
     RBLTINONEARG = 284,
     ID = 285,
     STRING = 286,
     HANDLER = 287,
     SYMBOL = 288,
     ENDCLAUSE = 289,
     tPLAYACCEL = 290,
     tDOWN = 291,
     tELSE = 292,
     tELSIF = 293,
     tEXIT = 294,
     tFRAME = 295,
     tGLOBAL = 296,
     tGO = 297,
     tIF = 298,
     tINTO = 299,
     tLOOP = 300,
     tMACRO = 301,
     tMOVIE = 302,
     tNEXT = 303,
     tOF = 304,
     tPREVIOUS = 305,
     tPUT = 306,
     tREPEAT = 307,
     tSET = 308,
     tTHEN = 309,
     tTO = 310,
     tWHEN = 311,
     tWITH = 312,
     tWHILE = 313,
     tNLELSE = 314,
     tFACTORY = 315,
     tMETHOD = 316,
     tOPEN = 317,
     tPLAY = 318,
     tDONE = 319,
     tINSTANCE = 320,
     tGE = 321,
     tLE = 322,
     tGT = 323,
     tLT = 324,
     tEQ = 325,
     tNEQ = 326,
     tAND = 327,
     tOR = 328,
     tNOT = 329,
     tMOD = 330,
     tAFTER = 331,
     tBEFORE = 332,
     tCONCAT = 333,
     tCONTAINS = 334,
     tSTARTS = 335,
     tCHAR = 336,
     tITEM = 337,
     tLINE = 338,
     tWORD = 339,
     tSPRITE = 340,
     tINTERSECTS = 341,
     tWITHIN = 342,
     tTELL = 343,
     tPROPERTY = 344,
     tON = 345,
     tME = 346,
     tENDIF = 347,
     tENDREPEAT = 348,
     tENDTELL = 349
   };
#endif
/* Tokens.  */
#define UNARY 258
#define CASTREF 259
#define VOID 260
#define VAR 261
#define POINT 262
#define RECT 263
#define ARRAY 264
#define OBJECT 265
#define REFERENCE 266
#define INT 267
#define ARGC 268
#define ARGCNORET 269
#define THEENTITY 270
#define THEENTITYWITHID 271
#define FLOAT 272
#define BLTIN 273
#define BLTINNOARGS 274
#define BLTINNOARGSORONE 275
#define BLTINONEARG 276
#define BLTINARGLIST 277
#define TWOWORDBUILTIN 278
#define FBLTIN 279
#define FBLTINNOARGS 280
#define FBLTINONEARG 281
#define FBLTINARGLIST 282
#define RBLTIN 283
#define RBLTINONEARG 284
#define ID 285
#define STRING 286
#define HANDLER 287
#define SYMBOL 288
#define ENDCLAUSE 289
#define tPLAYACCEL 290
#define tDOWN 291
#define tELSE 292
#define tELSIF 293
#define tEXIT 294
#define tFRAME 295
#define tGLOBAL 296
#define tGO 297
#define tIF 298
#define tINTO 299
#define tLOOP 300
#define tMACRO 301
#define tMOVIE 302
#define tNEXT 303
#define tOF 304
#define tPREVIOUS 305
#define tPUT 306
#define tREPEAT 307
#define tSET 308
#define tTHEN 309
#define tTO 310
#define tWHEN 311
#define tWITH 312
#define tWHILE 313
#define tNLELSE 314
#define tFACTORY 315
#define tMETHOD 316
#define tOPEN 317
#define tPLAY 318
#define tDONE 319
#define tINSTANCE 320
#define tGE 321
#define tLE 322
#define tGT 323
#define tLT 324
#define tEQ 325
#define tNEQ 326
#define tAND 327
#define tOR 328
#define tNOT 329
#define tMOD 330
#define tAFTER 331
#define tBEFORE 332
#define tCONCAT 333
#define tCONTAINS 334
#define tSTARTS 335
#define tCHAR 336
#define tITEM 337
#define tLINE 338
#define tWORD 339
#define tSPRITE 340
#define tINTERSECTS 341
#define tWITHIN 342
#define tTELL 343
#define tPROPERTY 344
#define tON 345
#define tME 346
#define tENDIF 347
#define tENDREPEAT 348
#define tENDTELL 349




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 79 "engines/director/lingo/lingo-gr.y"
{
	Common::String *s;
	int i;
	double f;
	int e[2];	// Entity + field
	int code;
	int narg;	/* number of arguments */
	Common::Array<double> *arr;
}
/* Line 1529 of yacc.c.  */
#line 247 "engines/director/lingo/lingo-gr.hpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;


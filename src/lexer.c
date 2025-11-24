/*
* Copyright © 2025 Thbop
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the “Software”), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
* of the Software, and to permit persons to whom the Software is furnished to do
* so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include "lexer.h"

#include "stdio.h"
#include "string.h"
#include "ctype.h"

#include "chelp/list_t.h" // For qalloc
#include "chelp/vector_t.h"
#include "chelp/tstring.h"

lexer_node_t *_node_new_list() {
    vector_t raw_list = new_vector( lexer_node_t );
    lexer_node_t node = {
        .type = LEXER_NODE_LIST,
        .data = qalloc( raw_list ),
    };
    return qalloc( node );
}

void _node_list_append( lexer_node_t *list, lexer_node_t *node ) {
    _vector_append( list->data, node );
}

int lexer_node_list_size( lexer_node_t *list ) {
    return ( (vector_t*)list->data )->elementCount;
}

lexer_node_t *lexer_node_list_at( lexer_node_t *list, unsigned int index ) {
    return _vector_at( list->data, index );
}

lexer_node_t *_node_new_token( char *source, int start, int end ) {
    lexer_node_t node = {
        .type = LEXER_NODE_TOKEN,
        .data = tstr_substr( source, start, end - start ),
    };

    return qalloc( node );
}


lexer_node_t *lexer_process_source( const char *source ) {
    if ( source == NULL )
        return NULL;

    lexer_node_t *root = _node_new_list();
    
    enum {
        STATE_NONE,
        STATE_TOKEN_ALNUM,
        STATE_TOKEN_SING,
        STATE_COMMENT,
        STATE_STRING,
    };
    int state = STATE_NONE;

    int token_start = 0;

    static const char singular_tokens[] = "(){};";

    for ( int i = 0; i < (int)strlen( source ); i++ ) {
        switch ( state ) {
            case STATE_NONE:
            _label_state_none:
                if ( isalnum( source[i] ) ) {
                    token_start = i;
                    state = STATE_TOKEN_ALNUM;
                }
                else if ( tstr_find( (char*)singular_tokens, source[i] ) > -1 ) {
                    _node_list_append(
                        root,
                        _node_new_token( (char*)source, i, i + 1 )
                    );
                }
                break;

            case STATE_TOKEN_ALNUM:
                if ( !isalnum( source[i] ) ) {
                    _node_list_append(
                        root,
                        _node_new_token( (char*)source, token_start, i )
                    );
                    state = STATE_NONE;
                    goto _label_state_none;
                }
                break;
        }
    }

    return root;
}


void _print_indent( int indent ) {
    for ( int i = 0; i < indent; i++ )
        putchar( ' ' );
}

void _print_node( lexer_node_t *node, int indent ) {
    switch ( node->type ) {
        case LEXER_NODE_LIST:
            for ( int i = 0; i < lexer_node_list_size( node ); i++ ) {
                _print_node( lexer_node_list_at( node, i ), indent + 2 );
            }
            break;
        case LEXER_NODE_TOKEN:
            _print_indent( indent );
            printf( "%s\n", (char*)node->data );
            break;
    }
}

void lexer_print_node( lexer_node_t *node ) {
    _print_node( node, 0 );
}

void lexer_free_node( lexer_node_t *node ) {
    if ( node->type == LEXER_NODE_LIST ) {
        for ( int i = 0; i < lexer_node_list_size( node ); i++ ) {
            lexer_free_node( lexer_node_list_at( node, i ) );
        }
        _vector_free( node->data );
    }
    free( node->data );
}
#include "parser.h"
#include "strutils.h"

ParserContext *create_parser_context(const char *filepath) {
  ParserContext *pctx = (ParserContext *)calloc(1, sizeof(ParserContext));
  pctx->stream = (filepath == NULL) ? stdin : fopen(filepath, "r");
  pctx->current_line = (char *)-1;
  pctx->current_state = ANY_STATE;
  pctx->transition = NULL;
  return pctx;
}

void parser_context_load_line(ParserContext *pctx) {
  if (getline(&pctx->current_line, &pctx->line_len, pctx->stream) == -1)
    parser_context_destroy_and_callback(pctx);
}

void parser_context_bind_trans(ParserContext *pctx, Transition *trans) {
  pctx->transition = trans;
}

bool parser_context_stream_eof(ParserContext *pctx) {
  return feof(pctx->stream);
}

void parse_markdown(const char *filepath) {
  ParserContext *pctx = create_parser_context(filepath);

  while (!parser_context_stream_eof(pctx)) {
    parser_context_load_line(pctx);

    while (pctx->current_line != NULL && *pctx->current_line != '\0') {
      Token current_token;
      if (match_token(&current_token, pctx->current_line)) {
        Transition *transition = &transition_table[pctx->current_state];

        if (transition->action != NO_ACTION) {
          ParseCallBackFn callback = pctx->action_callbacks[transition->action];
          callback(ctx_token, NULL);
        }

        pctx->current_state = transition->next_state;
        pctx->current_line += strlen(current_token.value);
      } else {
	pctx->current_state = ANY_STATE;
	continue;
      }
    }
  }

  parser_context_destroy(pctx);
}

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
  getline(&pctx->current_line[0], &pctx->line_len, pctx->stream);
  pctx->current_line[pctx->line_len] = '\0';
}

void parser_context_bind_trans(ParserContext *pctx, Transition *trans) {
  pctx->transition = trans;
}

bool parser_context_stream_eof(ParserContext *pctx) {
  return feof(pctx->stream);
}

void parser_context_destroy(ParserContext *pctx) {
  fclose(pctx->stream);
  free(pctx);
}

void parse_markdown(const char *filepath) {
  ParserContext *pctx = create_parser_context(filepath);

  while (!parser_context_stream_eof(pctx)) {
    parser_context_load_line(pctx);

    char *md_term = strtok(&pctx->current_line[0], " \t");
    while (md_term != NULL) {
      Transition *transition = &transition_table[pctx->current_state];
      Token *token = &transition->token;
      
      if (match_token(token, md_term)) {
        ParseCallbackFn callback = pctx->action_callbacks[transition->action];
        callback(md_term, pctx->current_state);
        pctx->current_state = transition->next_state;
      }

      md_term = strtok(NULL, " \t");
    }
  }

  parser_context_destroy(pctx);
}


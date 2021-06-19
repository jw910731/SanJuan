//
// Created by jw910731 on 2021/6/17.
//

#include "dummy.h"

/*
 * Client Meta exposing function prototype
 */
static int32_t init(struct sanjuan_client_meta *meta);
static void free_game(struct sanjuan_client_meta *meta);
static void pre_choose_role(struct sanjuan_client_meta *meta, const int8_t bitmask);
static enum sanjuan_role choose_role(struct sanjuan_client_meta *meta);
static int32_t idle(struct sanjuan_client_meta *meta);
static void pre_perform_role(struct sanjuan_client_meta *meta, enum sanjuan_role role, int32_t id);
static int32_t perform_role(struct sanjuan_client_meta *meta);
static int32_t perform_chapel(struct sanjuan_client_meta *meta);
// TODO: implement dummy ai functions

const struct sanjuan_client_meta sanjuan_dummyAI_client_meta={
    .init = init,
    .free = free_game,
    .pre_choose_role = pre_choose_role,
    .choose_role = choose_role,
    .idle = idle,
    .pre_perform_role = pre_perform_role,
    .perform_role = perform_role,
    .perform_chapel = perform_chapel,
};

static int32_t init(struct sanjuan_client_meta *meta){
    meta->fd = -1; // input fd is not used
}

static void free_game(struct sanjuan_client_meta *meta){

}


static void pre_choose_role(struct sanjuan_client_meta *meta, const int8_t bitmask){

}

static enum sanjuan_role choose_role(struct sanjuan_client_meta *meta){

}

static int32_t idle(struct sanjuan_client_meta *meta){

}

static void pre_perform_role(struct sanjuan_client_meta *meta, enum sanjuan_role role, int32_t id){

}

static int32_t perform_role(struct sanjuan_client_meta *meta){

}

static int32_t perform_chapel(struct sanjuan_client_meta *meta){

}
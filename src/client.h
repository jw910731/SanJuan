//
// Created by jw910731 on 2021/6/13.
//

#ifndef SANJUAN_ROOT_CLIENT_H
#define SANJUAN_ROOT_CLIENT_H

#include "game.h"

#include <stdint.h>

/*
 * Client interface struct
 */
struct sanjuan_client_meta{
    /*
     * ID of this client
     */
    // TODO: Design get id from game component
    int32_t id;

    /*
     * @return for error reporting
     */
    int (*init)();

    /*
     * Choose Role
     * @return chosen role
     */
    enum sanjuan_role (*choose_role)();

    // TODO: Design perform role function prototype
};

#endif //SANJUAN_ROOT_CLIENT_H

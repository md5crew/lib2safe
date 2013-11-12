#ifndef SAFECALLS_H
#define SAFECALLS_H

#define API_HOST "api.2safe.com"
#define TRUE "true"
#define FALSE "false"
#define MAX_THREADS 50
#define MAX_FILE_THREADS 8
/* CALLS SUPPORTED BY THE 2SAFE API */

/* USER */
#define CALL_CHECK_EMAIL "chk_mail"
#define CALL_CHECK_LOGIN "chk_login"
#define CALL_GET_CAPTCHA "get_captcha" // for 10min
#define CALL_REGISTER "add_login"
#define CALL_AUTH "auth" // 3 login fails before new captcha
#define CALL_UNREGISTER "remove_login"
#define CALL_LOGOUT "logout"
#define CALL_GET_DISK_QUOTA "get_disk_quota"
#define CALL_GET_PERSONAL "get_personal_data"
#define CALL_SET_PERSONAL "set_personal_data"
#define CALL_CHANGE_PASSWORD "change_password"
#define CALL_ACTIVATE_PROMO "activate_promo_code"

/* FILESYSTEM */
#define CALL_PULL_FILE "get_file"
#define CALL_PUSH_FILE "put_file"
#define CALL_COPY_FILE "copy_file"
#define CALL_MOVE_FILE "move_file"
#define CALL_REMOVE_FILE "remove_file"
#define CALL_MAKE_DIR "make_dir"
#define CALL_COPY_DIR "copy_dir"
#define CALL_MOVE_DIR "move_dir"
#define CALL_REMOVE_DIR "remove_dir"
#define CALL_LIST_DIR "list_dir"
#define CALL_GET_PROPS "get_props"
#define CALL_SET_PROPS "set_props"
#define CALL_GET_TREE  "get_tree_parent"
#define CALL_GET_EVENTS "get_events" // first 300 by default
#define CALL_PURGE_TRASH "purge_trash"

/* LOCKS */
#define CALL_LOCK "lock_object"
#define CALL_UNLOCK "unlock_object"
#define CALL_LIST_LOCKS "list_object_locks"
#define CALL_REFRESH_LOCK "refresh_lock_timeout"

/* SHARE */
#define CALL_SHARE "share_object"
#define CALL_UNSHARE "unshare_object"
#define CALL_UNSHARE_ALL  "unshare_all"
#define CALL_LIST_SHARES "list_shares"
#define CALL_PUBLIC "public_object"
#define CALL_UNPUBLIC "unpublic_object"

/* VERSIONS */
#define CALL_LIST_VERSIONS "list_versions"
#define CALL_GET_VERSION "get_current_version"
#define CALL_SET_VERSION "set_current_version"
#define CALL_REMOVE_VERSION "remove_version"
#define CALL_SET_VERSIONS_ON "set_versioned"
#define CALL_SET_VERSIONS_OFF "unset_versioned"

/* PARAMS SUPPORTED BY THE 2SAFE API */
#define PARAM_CMD "cmd"
#define PARAM_TOKEN "token" // 24 hours
#define PARAM_EMAIL "email"
#define PARAM_LOGIN "login"
#define PARAM_PASSWORD "password"
#define PARAM_NEW_PASSWORD "new_password"
#define PARAM_FORMAT "format"
#define PARAM_CAPTCHA "captcha"
#define PARAM_CAPTCHA_ID "id"
#define PARAM_CAPTCHA_KEY "key"
#define PARAM_PERSONAL "personal"
#define PARAM_PROPERTIES "props"
#define PARAM_PROMO_CODE "code"
#define PARAM_SIZE "size"
#define PARAM_OFFSET "offset"
#define PARAM_FILE "file"
#define PARAM_FILE_ID "id"
#define PARAM_URL "url"
#define PARAM_CONTAINER "container" // if PARAM_URL used
#define PARAM_DIR_ID "dir_id"
#define PARAM_DIR_NAME "dir_name"
#define PARAM_OVERWRITE "overwrite"
#define PARAM_VERSIONED "versioned"
#define PARAM_CREATE_TIME "ctime"
#define PARAM_MODIFY_TIME "mtime"
#define PARAM_FILE_NAME "name"
#define PARAM_NEW_FILE_NAME "file_name"
#define PARAM_EVENTS_AFTER "after"
#define PARAM_LAST_EVENT "last"
#define PARAM_LOCK_EXCLUSIVE "exclusive"
#define PARAM_LOCK_TIMEOUT "timeout"
#define PARAM_LOCK_USERDATA "userdata"
#define PARAM_LOCK_TOKEN "lock_token"
#define PARAM_SHARE_WRITABLE "write"
#define PARAM_SHARE_EXPIRES "expires"
#define PARAM_VERSION_ID "v_id"


#endif // SAFECALLS_H

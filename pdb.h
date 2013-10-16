#ifndef PDB_H
#define PDB_H

const char *field_names[] = {
    "Gender",
    "Name First",
    "Initial Mid",
    "Name Last",
    "City",
    "State",
    "Country",
    "Email",
    "Username",
    "Password",
    "Telephone",
    "Date of Birth",
    "Occupation",
    "Company",
    "Blood Type",
    "Weight",
    "Height",
    "Credit Card Type",
    "Credit Card Number",
    "Credit Card CCV2",
    "Credit Card Expiration Date",
    NULL,
};

typedef enum field_type_t{
    GENDER,
    NAME_FIRST,
    INITIAL_MID,
    NAME_LAST,
    CITY,
    STATE,
    COUNTRY,
    EMAIL,
    USERNAME,
    PASSWORD,
    TELEPHONE,
    BIRTHDAY,
    OCCUPATION,
    COMPANY,
    BLOOD_TYPE,
    WEIGHT,
    HEIGHT,
    CC_TYPE,
    CC_NUM,
    CC_CVV2,
    CC_EXP
}field_type_t;

typedef struct field_t{
    char *field_name;
    char *field_entry;
    field_type_t type;
    unsigned short field_id;
    unsigned short parent_table;
    struct field_t *next;
}field_t;

typedef struct table_t{
    field_t *fields;
    unsigned short table_id;
    unsigned short field_count;
    struct table_t *next;
}table_t;

#endif

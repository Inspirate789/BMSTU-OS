struct REQUEST
{
    int index;
    int number;
    int pid;
};

program BAKERY_PROG
{
    version BAKERY_VER
    {
        int GET_NUMBER(struct REQUEST) = 1;
        int BAKERY_SERVICE(struct REQUEST) = 2;
    } = 1; /* Version number = 1 */
} = 0x20000001; /* RPC program number */

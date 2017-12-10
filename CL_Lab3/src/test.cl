__kernel void test(__global int* message)
{
	// получаем текущий id.
	int gid = get_global_id(0);
    int gg = gid;
    int gv = message[gg];
    int rv = 0;
    if(gv<1){
        message[gg] = -1;
        return;
    }
    if(gv==1){
        message[gg]=0;
        return;
    }
    //message[gg] = 0;
    
    while(gv>1){
        if(gv%2){
            rv = -1;
            break;
        }
        gv /= 2;
        rv++;        
    }
    message[gg] = rv;
	//message[gid] = gid;
    
}

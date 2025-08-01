# Platform manual


# how to add new platform

``` C

enum{
    COMMON_FUNCTION_LIKE_PRINT = 0, 
    //...
    PLATFORM_ORIGINAL_FUNCTION = 126,
    //...
};
struct EventTable __EventTable__[] ={

}
struct StdFuncTable __StdFuncTable__[] ={
    
}
```
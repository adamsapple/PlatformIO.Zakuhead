/**
 * 静的配列の長さを得る
 */
template<typename TYPE, size_t SIZE>
size_t array_length(const TYPE (&)[SIZE])
{
    return SIZE;
}

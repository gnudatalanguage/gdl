// template<typename Sp>
// template<typename U>
// typename Sp::Ty Data_<Sp>::B1() 
// {
//     std::cout << "is_integer\n";
//     return Sp::zero;
// }
// template<typename Sp>
// template<typename U>
// typename std::enable_if<is_integer<typename U::Ty>::value, typename U::Ty>::type
// Data_<Sp>::Test1() 
// {
//   dd[0]++;
//   dd[0]--;
//     std::cout << "is_integer\n";
//     return Sp::zero;
// }
// template<typename Sp>
// template<typename U>
// typename std::enable_if<is_float<typename U::Ty>::value, typename U::Ty>::type
// Data_<Sp>::Test1() 
// {
//     std::cout << "is_float\n";
//     return Sp::zero;
// }
// template<typename Sp>
// template<typename U>
// typename std::enable_if<is_complex<typename U::Ty>::value, typename U::Ty>::type
// Data_<Sp>::Test1() 
// {
//     std::cout << "is_complex\n";
//     return Sp::zero;
// }
// template<typename Sp>
// template<typename U>
// typename std::enable_if<is_other<typename U::Ty>::value, typename U::Ty>::type 
// Data_<Sp>::Test1() 
// {
//     std::cout << "is_other\n";
//     return Sp::zero;
// }
 
template<typename Sp>
template<typename U>
typename U::template IfInteger<bool>::type 
Data_<Sp>::Test2() 
{
  dd[0]++;
  dd[0]--;
    std::cout << "is_integer\n";
    return true;
}
template<typename Sp>
template<typename U>
typename U::template IfFloat<bool>::type 
Data_<Sp>::Test2() 
{
    std::cout << "is_float\n";
    return true;
}
template<typename Sp>
template<typename U>
typename U::template IfComplex<bool>::type 
Data_<Sp>::Test2() 
{
    std::cout << "is_complex\n";
    return true;
}
template<typename Sp>
template<typename U>
typename U::template IfOther<bool>::type 
Data_<Sp>::Test2() 
{
    std::cout << "is_other\n";
    return false;
}
 





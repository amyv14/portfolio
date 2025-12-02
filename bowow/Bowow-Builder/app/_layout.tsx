import { Stack } from 'expo-router/stack';
import { CartProvider } from './cartcontext';


export default function Layout() {
  return (
    <CartProvider>
      <Stack screenOptions={{headerShown: false}}>
        <Stack.Screen name="(tabs)" options={{ headerShown: false }} />
      </Stack>
    </CartProvider>
  );
}

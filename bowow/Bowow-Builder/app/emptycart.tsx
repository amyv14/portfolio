import {StyleSheet, View, Text, StatusBar, TouchableOpacity} from 'react-native';
import React from 'react'
import {SafeAreaView, SafeAreaProvider} from 'react-native-safe-area-context';
import { Image } from 'expo-image';
import {Link} from 'expo-router';

// blurhash for placeholder image loading
const blurhash =
  '|rF?hV%2WCj[ayj[a|j[az_NaeWBj@ayfRayfQfQM{M|azj[azf6fQfQfQIpWXofj[ayj[j[fQayWCoeoeaya}j[ayfQa{oLj?j[WVj[ayayj[fQoff7azayj[ayj[j[ayofayayayj[fQj[ayayj[ayfjj[j[ayjuayj[';

// empty cart component that shows when the cart is empty
const EmptyCart = () => {
    return (
        <SafeAreaProvider>
        <SafeAreaView style={styles.container} edges={['top']}>
        <View style={styles.container}>
      <Image
        style={styles.image}
        // source={require("../assets/images/emptycart.webp")}
        placeholder={{ blurhash }}  // using blurhash as a placeholder for the image
        contentFit="cover"
        transition={1000}
      />
      <Text style= {styles.text}>Ohh... Your Bow-wow is Empty</Text>
      <Text style= {styles.smallertext}>but it doesn't have to be. </Text>
      {/* link to the shop page */}
      <Link href="/shop" style={styles.button}>
        <Text style={styles.buttonText}>SHOP NOW</Text>
      </Link>  
    </View>
        </SafeAreaView>
      </SafeAreaProvider>
    )
}

// styles for the empty cart screen
const styles = StyleSheet.create({
  textBox: {
    padding: 20
  },
  container: {
    flex: 1,
    paddingTop: StatusBar.currentHeight,
    justifyContent: 'center',
    alignItems: 'center',
  },
  text: {
    fontSize: 20,
    marginTop: 20,
    color: 'dark grey'
  },
  smallertext: {
    fontSize: 20,
    marginTop: 5,
    color: 'grey',
    marginBottom: 20
  },
  buttonLink: {
    textDecorationLine: 'none',  
  }, button: {
    backgroundColor: '#007bff',
    paddingVertical: 12,
    paddingHorizontal: 30,
    borderRadius: 5,
    justifyContent: 'center',
    alignItems: 'center',
  },
  buttonText: {
    fontSize: 18,
    color: 'white',
    fontWeight: 'bold',
  },
  image: {
    width: 300,
    height: 300,
    resizeMode: 'contain',
    justifyContent: 'center',
    alignItems: 'center',
  },
});

export default EmptyCart;

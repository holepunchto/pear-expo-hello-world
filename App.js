import { StatusBar } from 'expo-status-bar';
import { useCallback, useState } from 'react';
import { Button, StyleSheet, Text, TextInput, View } from 'react-native';
import { reverseString, /*upperString*/ } from './lib/native';

export default function App() {
  const [value, setValue] = useState('')
  const [reversed, setReversed]  = useState(null)

  const btnCta = useCallback(() => {
    reverseString(value).then(result => {
      setReversed(result)
    })
    // upperString(value).then(result => {
    //   setReversed(result)
    // })
    setValue("")
  }, [value, reverseString, setReversed, setValue])

  return (
    <View style={styles.container}>
      <Text style={styles.text}>Reverse string in bare:</Text>
      <TextInput style={styles.input} value={value} onChangeText={setValue}/>
      <Button
        onPress={btnCta}
        title="Send"
      />
      {reversed !== null && <><Text style={styles.text}>Result: {reversed}</Text></>}
      <StatusBar style="auto" />
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#f1f1f1',
    alignItems: 'center',
    justifyContent: 'center',
    gap: 16
  },
  text: {
    fontSize: 16
  },
  input: {
    width: 300,
    padding: 10,
    backgroundColor: '#fff'
  }
});
